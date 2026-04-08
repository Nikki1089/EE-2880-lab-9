/**
 * Driver for ping sensor
 * @file ping.c
 * @author Lucas England
 */

#include "ping.h"
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>
#include <driverlib/interrupt.h>
#include <stdint.h>

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile uint32_t g_pulse_tick = 0;
volatile uint32_t g_overflow = 0;
volatile enum
{ LOW, HIGH, DONE } g_state = LOW; // State of ping echo pulse

void ping_init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x08;

    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}
    while ((SYSCTL_RCGCTIMER_R & 0x08) == 0) {}
   

    GPIO_PORTB_DIR_R &= ~0x08;          // PB3 input
    GPIO_PORTB_DEN_R |= 0x08;           // Enable digital on PB3
    GPIO_PORTB_AFSEL_R |= 0x08;         // Enable alt function on PB3
    GPIO_PORTB_PCTL_R &= ~0x0000F000;   // Clear PB3 nibble
    GPIO_PORTB_PCTL_R |= 0x00007000;    // PB3 = T3CCP1
    GPIO_PORTB_AMSEL_R &= ~0x08;        // Disable analog on PB3

    TIMER3_CTL_R &= ~0x0100;            // Disable Timer3B during setup
    TIMER3_CFG_R = 0x04;                // 16-bit timer mode
    TIMER3_TBMR_R = 0x07;               // Capture mode, edge-time mode, count down
    TIMER3_CTL_R |= 0x0C00;             // Capture both edges
    TIMER3_TBILR_R = 0xFFFF;            // Lower 16 bits max
    TIMER3_TBPR_R = 0xFF;               // Prescaler upper 8 bits max

    TIMER3_ICR_R |= 0x400;              // Clear Timer3B capture flag
    TIMER3_IMR_R |= 0x400;  

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x100;
}

void ping_trigger(void)
{
    g_state = LOW;
    g_overflow =0;
    g_pulse_ticks = 0;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x8;
    GPIO_PORTB_DIR_R |= 0x8;
    GPIO_PORTB_DATA_R &= ~0x08;
    timer_waitMicros(5);
    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08;
    GPIO_PORTB_DIR_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x8;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
}

void TIMER3B_Handler(void)
{

    // YOUR CODE HERE
    // As needed, go back to review your interrupt handler code for the UART lab.
    // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
    // includes checking the source of the interrupt and clearing the interrupt status bit.
    // Checking the source: test the MIS bit in the MIS register (is the ISR executing
    // because the input capture event happened and interrupts were enabled for that event?
    // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
    // The rest of the code in the ISR depends on actions needed when the event happens.

    if (TIMER3_MIS_R & 0x0400)
    {
        TIMER3_ICR_R |= 0x0400;
//        lcd_printf("aeiou");
        switch (g_state)
        {
        case LOW:
            g_start_time = TIMER3_TBV_R;
            g_state = HIGH;
            break;
        case HIGH:
            g_end_time = TIMER3_TBV_R;
            g_state = DONE;
            break;
        default:
            break;
        }
    }

}
uint32_t ping_getPulseTicks(void){
while(g_state != DONE){}
if(g_start_time >= g_end_time){
g_pulse_ticks = g_start_time - g_end_time;
}
else {
g_overflow = 1;
}
return g_pulse_ticks;
}

float ping_getDistance(void)
{
    uint32_t pulse_ticks;
    float time;
    float distance;
    pulse_ticks = ping_getPulseTicks();
    time = pulse_ticks / 16000000;
    distance = (time * 34300) / 2;
    return distance;
}
 

    return (float)(g_end_time - g_start_time) * 10000;

}
uint32_t get_g_start_time(void){
    return g_start_time;
}

uint32_t get_g_end_time(void){
    return g_end_time;
}


