/**
 * Driver for ping sensor
 * @file ping.c
 * @author Lucas England
 */

#include "ping.h"
#include "Timer.h"
#include "lcd.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile enum
{
    LOW, HIGH, DONE
} g_state = LOW; // State of ping echo pulse

void ping_init(void)
{

    //enable clock to GPIO port B
    SYSCTL_RCGCGPIO_R |= 0b000010;
    while ((SYSCTL_PRGPIO_R & 0b000010) == 0)
    {
    };
    SYSCTL_RCGCTIMER_R |= 0b001000;
    while ((SYSCTL_RCGCTIMER_R & 0b001000) == 0)
    {
    };
    GPIO_PORTB_DEN_R |= 0b001000;

    lcd_printf("0\n");
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xf000) | 0x7000;
    lcd_printf("1\n");
    TIMER3_CFG_R = (TIMER3_CFG_R & ~0x07) | 0x00;
    lcd_printf("2\n");
    TIMER3_TBMR_R &= ~0x13;
    lcd_printf("3\n");
    GPIO_PORTB_AFSEL_R |= 0x8;
    lcd_printf("4\n");
    TIMER3_IMR_R |= 0x100;
    lcd_printf("5\n");
    TIMER3_CTL_R |= 0xd00;
    lcd_printf("complete\n");

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x100;
}

void ping_trigger(void)
{
    g_state = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x8;
    GPIO_PORTB_DIR_R |= 0x8;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08;
    GPIO_PORTB_DATA_R |= 0x08;
    GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_DIR_R &= ~0x8;
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

uint32_t get_g_start_time(void){
    return g_start_time;
}

uint32_t get_g_end_time(void){
    return g_end_time;
}

float ping_getDistance(void)
{

    // YOUR CODE HERE

    return (float)(g_end_time - g_start_time) * 10000;

}

