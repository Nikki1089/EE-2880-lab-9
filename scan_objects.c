#include "scan_objects.h"

#define cyBot_sendf(...) uart_printf(__VA_ARGS__)

void sweep_ping(cyBOT_Scan_t sensors[SWEEP_COUNT])
{
    int i = 0, obj_count = 0, current_start = 3 * SWEEP_STEP, current_width = 0,
            smallest = 0;
    object_t objs[MAX_OBJS];
    for (i = 0; i < 5; ++i)
    {
        cyBOT_Scan(0, &(sensors[0]));
    }
    for (i = 0; i * SWEEP_STEP + SWEEP_START <= SWEEP_END; ++i)
    {
        float distances[3];
        cyBot_sendf("%d: ", i * SWEEP_STEP + SWEEP_START);
        cyBOT_Scan(i * SWEEP_STEP + SWEEP_START, &(sensors[i]));
        distances[0] = sensors[i].sound_dist;
        cyBot_sendf("%.1f ", distances[0]);
        cyBOT_Scan(i * SWEEP_STEP + SWEEP_START, &(sensors[i]));
        distances[1] = sensors[i].sound_dist;
        cyBot_sendf("%.1f ", distances[1]);
        cyBOT_Scan(i * SWEEP_STEP + SWEEP_START, &(sensors[i]));
        distances[2] = sensors[i].sound_dist;
        cyBot_sendf("%.1f ", distances[2]);
        sensors[i].sound_dist = (distances[0] + distances[1] + distances[2])
                / 3;
        cyBot_sendf("%.1f\r\n", sensors[i].sound_dist);
    }

    // analyze the scans to find objects
    for (i = 3; i <= SWEEP_COUNT; ++i)
    {
        if (sensors[i].sound_dist > CUTOFF_DIST || i == SWEEP_COUNT)
        {
            if (sensors[i - 1].sound_dist < CUTOFF_DIST)
            {
                cyBot_sendf(
                        "Detected an object starting at angle %d with an angular width of %d\r\n",
                        current_start, current_width);
                if (obj_count >= MAX_OBJS)
                {
                    cyBot_sendf("Object array is full!!!");
                }
                else
                {
                    objs[obj_count].start = current_start;
                    objs[obj_count].width = current_width;
                    objs[obj_count].end = i * SWEEP_STEP + SWEEP_START;
                }
                ++obj_count;
            }
            current_width = 0;
        }
        else
        {
            if (sensors[i - 1].sound_dist > CUTOFF_DIST)
            {
                current_start = i * SWEEP_STEP + SWEEP_START;
            }
            current_width += SWEEP_STEP;
        }
    }
    for (i = 0; i < obj_count && i < MAX_OBJS; ++i)
    {
        cyBot_sendf("Object %d:\r\n\tstart: %d\r\n\tend: %d\r\n\twidth: %d\r\n",
                    i, objs[i].start, objs[i].end, objs[i].width);
        if (objs[i].width < objs[smallest].width)
            smallest = i;
    }
    cyBot_sendf(
            "The smallest object starts at angle %d and has an angular width of %d\r\n",
            objs[smallest].start, objs[smallest].width);
    cyBOT_Scan(objs[smallest].start + objs[smallest].width / 2, &(sensors[0]));

}

//int ir_get_dist(int ir_raw) {
//    return CUTOFF_DIST - ir_raw
//}

size_t sweep_ir(object_t objs[MAX_OBJS])
{
    int i, j, obj_count = 0, current_start = 3 * SWEEP_STEP, current_width = 0,
            ir_vals[SWEEP_COUNT];
    cyBOT_Scan_t scan;

    // position scanner
    for (i = 0; i < 5; ++i)
    {
        cyBOT_Scan(0, &scan);
    }

    uart_printf("\n");

    // first (IR) pass
    for (i = 0; i * SWEEP_STEP + SWEEP_START <= SWEEP_END; ++i)
    {
        ir_vals[i] = 0;
        uart_printf("%d ", i * SWEEP_STEP + SWEEP_START);
        for (j = 0; j < SCAN_COUNT; ++j)
        {
            cyBOT_Scan(i * SWEEP_STEP + SWEEP_START, &scan);
            ir_vals[i] += scan.IR_raw_val;
            uart_printf("%d ", scan.IR_raw_val);
        }

        ir_vals[i] /= SCAN_COUNT;
        uart_printf("\n"); //, (2 * PI * ir_vals[i]) / 180

        if (ir_vals[i] < IR_CUTOFF || i == SWEEP_COUNT)
        {
            if (ir_vals[i - 1] > IR_CUTOFF)
            {
//                uart_printf(
//                        "Detected an object starting at angle %d with an angular width of %d\n",
//                        current_start, current_width);
                if (obj_count >= MAX_OBJS)
                {
                    uart_printf("Object array is full!!!");
                }
                else
                {
                    objs[obj_count].start = current_start;
                    objs[obj_count].width = current_width;
                    objs[obj_count].end = i * SWEEP_STEP + SWEEP_START;
                }
                ++obj_count;
            }
            current_width = 0;
        }
        else
        {
            if (ir_vals[i - 1] < IR_CUTOFF)
            {
                current_start = i * SWEEP_STEP + SWEEP_START;
            }
            current_width += SWEEP_STEP;
        }
    }

    // second (PING) pass
    for (i = 0; i < obj_count; ++i)
    {
        uart_printf("%i ", objs[i].start + objs[i].width / 2);
        cyBOT_Scan(objs[i].start + objs[i].width / 2, &scan);
        cyBOT_Scan(objs[i].start + objs[i].width / 2, &scan);
        cyBOT_Scan(objs[i].start + objs[i].width / 2, &scan);
//        uart_printf("Pinging object %d (angle %d)... ", i,
//                    objs[i].start + objs[i].width / 2);
        objs[i].dist = 0;
        for (j = 0; j < PING_COUNT; ++j){

        cyBOT_Scan(objs[i].start + objs[i].width / 2, &scan);
        objs[i].dist += scan.sound_dist;
        uart_printf("%.1f ", scan.sound_dist);
        }
        objs[i].dist /= PING_COUNT;
//        uart_printf("%f cm away\n", objs[i].dist);
        uart_printf("%f\n", objs[i].dist);
    }
    return obj_count;
}

int get_smallest_i(size_t objc, object_t objs[MAX_OBJS])
{
    int i;
    int smallest_i = 0;
    float smallest_width = ((objs[0].width * PI * objs[0].dist) / 180);
    for (i = 0; i < objc; ++i)
    {
        uart_printf(
                "%d (%d-%d): %d degrees from %f cm away, linear width %f cm\n",
                i, objs[i].start, objs[i].end, objs[i].width, objs[i].dist,
                LINEAR_WIDTH(objs[i]));
        if (LINEAR_WIDTH(objs[i]) < smallest_width)
        {
            smallest_width = LINEAR_WIDTH(objs[i]);
            smallest_i = i;
        }
    }
    uart_printf("\nthe smallest object is object %d, with a width of %f\n\n",
                smallest_i, smallest_width);
    return smallest_i;
}

//object_t *get_smallest(size_t objc, object_t objs[MAX_OBJS]) {
//
//}
