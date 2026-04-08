#ifndef SCAN_OBJECTS_H_
#define SCAN_OBJECTS_H_


#include "cyBot_Scan.h"
#include "uart-interrupt.h"

#include "open_interface.h"
#include "movement.h"

#define SWEEP_START 0
#define SWEEP_END 180
#define SWEEP_STEP 2
#define SWEEP_COUNT 1 + (SWEEP_END - SWEEP_START) / SWEEP_STEP

#define EDGE_DELTA 16
#define CUTOFF_DIST 50

#define MAX_OBJS 8

#define IR_CUTOFF 925
#define SCAN_COUNT 1
#define PING_COUNT 10

#define PI 3.1415926535897
#define LINEAR_WIDTH(o) ((o.width * PI * o.dist) / 180.0)
#define MIDPOINT(o) (o.start + o.width / 2)

typedef struct {
	int start, end, width;
	float dist;
} object_t;

void sweep_ping(cyBOT_Scan_t sensors[SWEEP_COUNT]);

size_t sweep_ir(object_t objs[MAX_OBJS]);

int get_smallest_i(size_t objc, object_t objs[MAX_OBJS]);

#endif
