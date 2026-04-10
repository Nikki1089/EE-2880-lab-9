/**
 * Driver for ping sensor
 * @file ping.c
 * @author Lucas England
 */

#include "ping.h"
#include "Timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include <driverlib/interrupt.h>
#include <stdint.h>

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile uint8_t g_overflow = 0;
volatile uint32_t g_pulse_ticks = 0;
volatile enum
{
    LOW, HIGH, DONE
} g_state = LOW; // State of ping echo pulse

void ping_init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x08;

    while ((SYSCTL_PRGPIO_R & 0x02) == 0)
    {
    }
    while ((SYSCTL_PRTIMER_R & 0x08) == 0)
    {
    }

//    *GPIO_PORTB_DIR_R &= ~0x08; //pb3 input
    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_AFSEL_R |= 0x8;
    GPIO_PORTB_PCTL_R &= ~0x0000f000;
    GPIO_PORTB_PCTL_R |= 0x000007000;
//    GPIO_PORTB_AMSEL_R &= ~0x08;

    TIMER3_CTL_R &= ~0x100;
    TIMER3_CFG_R = 0x4;
    TIMER3_TBMR_R |= 0x7; //capture
    TIMER3_TBMR_R &= ~0x10;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0xc00;
    TIMER3_TBILR_R |= 0xffff;
    TIMER3_TBPR_R |= 0xff;

    TIMER3_ICR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;

    //NVIC setup: set priority of UART1 interrupt to 1 in bits 21-23
//    NVIC_PRI1_R &= ~0x000000F0;
//    NVIC_PRI1_R |= 0x00000020;
    //NVIC setup: enable interrupt for UART1, IRQ #6, set bit 6
    NVIC_EN1_R |= 0x10;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
}

void ping_trigger(void)
{
    g_state = LOW;
    g_overflow = 0;
    g_pulse_ticks = 0;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x8;
//    GPIO_PORTB_DIR_R |= 0x8;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R |= 0x08;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
//    GPIO_PORTB_DIR_R &= ~0x8;
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

    if (TIMER3_MIS_R & 0x400)
    {
        TIMER3_ICR_R |= 0x400;
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

uint32_t ping_getPulseTicks(void)
{
    while (g_state != DONE)
    {
    }
    if (g_start_time >= g_end_time)
    {
        g_pulse_ticks = g_start_time - g_end_time;
        g_overflow = 0;
    }
    else
    {
        g_overflow = 1;
//        g_pulse_ticks = g_
    }
    return g_pulse_ticks;
}

float ping_getDistance(void)
{
    uint32_t pulse_ticks;
    float time_s;
    float distance_cm;
    pulse_ticks = ping_getPulseTicks();
    time_s = pulse_ticks / 16000000.0;
    distance_cm = (time_s * 34300) / 2.0;
    return distance_cm;
}

uint32_t get_g_start_time(void)
{
    return g_start_time;
}

uint32_t get_g_end_time(void)
{
    return g_end_time;
}

//float ping_getDistance(void)
//{
//
//    // YOUR CODE HERE
//
//    return (float) g_end_time - (float) g_start_time;
//
//}
