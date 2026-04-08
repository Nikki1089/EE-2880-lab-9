/**
 * main.c
 *
 * Main file for CprE 288 Lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 * @author Diane Rover, updated 2/25/2021, 2/17/2022
 *
 * @author Lucas England
 */

#define LAB_9
#define BOT_4
#define NONAV

#ifdef LAB_7
#include "movement.h"
#include "button.h"
#include "timer.h"
#include "lcd.h"

#include "uart-interrupt.h"

#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors
#include "scan_objects.h"
#include "adc.h"

#define PI 3.1415926535897
#define LINEAR_WIDTH(o) ((o.width * PI * o.dist) / 180.0)
#define SPEED (MAX_SPEED / 4)
#define TURN_SPEED (SPEED / 3)

int main(void) {

	button_init();
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();

	// initialize the cyBot UART1 before trying to use it
	uart_interrupt_init();

	// Initialize the scan
	cyBOT_init_Scan(0b0111);
	// Remember servo calibration function and variables from Lab 3

#ifdef BOT_4
	right_calibration_value = 290500;
	left_calibration_value = 1288000;
#endif

#ifdef BOT_16
	right_calibration_value = 227500; // 290500;
	left_calibration_value = 1183000; // 1288000;
#endif

	object_t objs[MAX_OBJS];
//    int i, smallest_i = 0;
//    float smallest_width;
	size_t objc;
	int timeout, smallest_i;
	float distleft;
	oi_t *bump = oi_alloc();
	oi_init(bump);

	lcd_printf("ready");
    uart_printf("ready\n");
	while (1) {

//        char cmd = uart_receive();
//        // echo the received key back to PC
//        uart_sendChar(cmd);
//        // uart_sendChar('\n');
//        // uart_sendf("Received a '%c'\r\n", cmd);

		if (command_flag) {
			char cmd = command_byte;
			command_flag = 0;

			switch (cmd) {
			case CMD_SERVO_CAL:
				cyBOT_SERVO_cal();
				break;
			case CMD_SWEEP_IR:
				distleft = 10000;
				for (timeout = 0; distleft > 10 && timeout < 10; ++timeout) {
					objc = sweep_ir(objs);
					if (objc <= 0) {
						uart_printf("No objects detected\n");
						break;
					}
#ifndef NONAV
					smallest_i = get_smallest_i(objc, objs);
					distleft = objs[smallest_i].dist * 10;
					uart_printf(
						"navigating towards object %d (angle: %d, distance: %f mm)\n",
						smallest_i, MIDPOINT(objs[smallest_i]), distleft);
					if (MIDPOINT(objs[smallest_i]) > 90) {
						uart_printf("turning %d degrees counterclockwise\n",
							MIDPOINT(objs[smallest_i]) - 90);
						turn_ccw(bump, MIDPOINT(objs[smallest_i]) - 90,
							TURN_SPEED);
					} else if (MIDPOINT(objs[smallest_i]) < 90) {
						uart_printf("turning %d degrees clockwise\n",
							90 - MIDPOINT(objs[smallest_i]));
						turn_cw(bump, 90 - MIDPOINT(objs[smallest_i]),
							TURN_SPEED);
					}
					distleft = move_forward(bump, distleft, SPEED, 0);
					uart_printf("%f mm remaining (%d|%d)\n", distleft,
						bump->bumpLeft, bump->bumpRight);
					if (distleft < 0)
						break;
					else if (bump->bumpLeft) {
						move_backward(bump, 50, SPEED, 1);
						turn_cw(bump, 90, TURN_SPEED);
						move_forward(bump, 250, SPEED, 1);
						turn_ccw(bump, 90, TURN_SPEED);
					} else if (bump->bumpRight) {
						move_backward(bump, 50, SPEED, 1);
						turn_ccw(bump, 90, TURN_SPEED);
						move_fwd(bump, 100, SPEED, 1);
						turn_cw(bump, 90, TURN_SPEED);
					}
					oi_update(bump);
					uart_printf("%f\n", bump->distance);
#else
					break;
#endif
				}
				break;

			default:
				break;
			}
		}
	}
}
#endif

#ifdef LAB_7A

///////////////////////////////////////////////////////////////////////
// Name: Simple-CyBot-echo.c                                         //
// Author: Phillip Jones (phjones@iastate.edu)                       //
// Date: 09/26/2023                                                  //
// Description: Simple Cybot program that is an Echo'ish program for //
//              communicating with the Starter Client Socket code.   //
///////////////////////////////////////////////////////////////////////

#include "Timer.h"
#include "lcd.h"
#include "uart-interrupt.h"

int main(void)
{

    timer_init(); // Initialize Timer, needed before any LCD screen fucntions can be called
                  // and enables time functions (e.g. timer_waitMillis)
    lcd_init();  // Initialize the the LCD screen.  This also clears the screen.
    cyBot_uart_init();  // Initialize UART

    char my_data;       // Variable to get bytes from Client
    char command[100];  // Buffer to store command from Client
    int index = 0;      // Index position within the command buffer

    // Write to LCD so that we know the program is running
    lcd_printf("Running");

    while (1)
    {

        index = 0;  // Set index to the beginning of the command buffer
        my_data = cyBot_getByte(); // Get first byte of the command from the Client

        // Get the rest of the command until a newline byte (i.e., '\n') received
        while (my_data != '\n')
        {
            command[index] = my_data;  // Place byte into the command buffer
            index++;
            my_data = cyBot_getByte(); // Get the next byte of the command
        }

        command[index] = '\n'; // place newline into command in case one wants to echo the full command back to the Client
        command[index + 1] = 0; // End command C-string with a NULL byte so that functions like printf know when to stop printing

        lcd_printf("Got: %s", command); // Print received command to the LCD screen

        // Send a response to the Client (Starter Client expects the response to end with \n)
        // In this case I am just sending back the first byte of the command received and a '\n'
        cyBot_sendByte(command[0]);

        // Only send a '\n' if the first byte of the command is not a '\n',
        // to avoid sending back-to-back '\n' to the client
        if (command[0] != '\n')
        {
            cyBot_sendByte('\n');
        }

    }

    return 0;
}

#endif

#ifdef LAB_8
#include "button.h"
#include "timer.h"
#include "lcd.h"
#include "uart-interrupt.h"
#include "adc.h"


int main(void) {
    timer_init();
    lcd_init();
    uart_interrupt_init();
    adc_init();

	while (1)
    {
        uint16_t adc_value = adc_read(); // change adc_read() to adc_read_average()
        float dist = adc_dist();
        uint16_t raw_avg = adc_avg(); // change adc_read() to adc_read_average()
        float dist_avg = adc_avgd();
        lcd_printf("adc: %4u\n     %6.1f\n     %4u\n     %6.1f", adc_value, dist, raw_avg, dist_avg);//,adc_value,dist); for part 2 plus change formatting to add dist:
        timer_waitMillis(1000);
    }
}
#endif

#ifdef LAB_9
#include "timer.h"
#include "lcd.h"
#include "ping.h"

// Uncomment or add any include directives that are needed

//#warning "Possible unimplemented functions"
//#define REPLACEME 0

int main(void) {
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	ping_init();

	// YOUR CODE HERE

    lcd_printf("aeiou");
	while (1) {
	    ping_trigger();
        timer_waitMillis(2000);
//	    lcd_printf("%d\n%d\n%f\n", get_g_start_time(), get_g_end_time(), ping_getDistance());
	}
}

#endif

#ifdef LAB_10
#include "timer.h"
#include "lcd.h"
#include "ping.h"

// Uncomment or add any include directives that are needed

//#warning "Possible unimplemented functions"
//#define REPLACEME 0

int main(void) {
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    ping_init();

    // YOUR CODE HERE

    lcd_printf("aeiou");
    while (1) {
        ping_trigger();
        timer_waitMillis(2000);
    }
}

#endif
