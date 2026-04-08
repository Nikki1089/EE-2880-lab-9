//#include "adc.h"
//#include <stdint.h>
//#include <inc/tm4c123gh6pm.h>
///**
// * Initialize ADC for Lab 8, Part 1.
// * Written by Lucas England.
// *
// * in adc.h:
// * ```
// * #define ADC_MODULE_0 1
// * #define ADC_SS_0 1
// * ```
// */
//void adc_init()
//{
//    // enables clocks for adc0 and port b
//    SYSCTL_RCGCADC_R |= 0x1;
//    SYSCTL_RCGCGPIO_R |= 0x02;
//
//    while ((SYSCTL_PRGPIO_R & 0x02) == 0)
//    { // waiting until GPIO port b is ready
//        /* do nothing */
//    };
//// configures pb4 for analog inputs
//    GPIO_PORTB_DIR_R &= ~0x10; // sets pb4 as input
//    GPIO_PORTB_AFSEL_R |= 0x10; //enables alternate function
//    GPIO_PORTB_DEN_R &= ~0x10; // disables digital
//    GPIO_PORTB_AMSEL_R |= 0x10; // enables analog
//
//    while (!(SYSCTL_PRADC_R & ADC_MODULE_0))
//    {
//        /* do nothing */
//    }
//
//    ADC0_PC_R &= ~0xF;
//    ADC0_PC_R |= 0x1;
//
//    ADC0_ACTSS_R &= ~ADC_SS_0;
//    ADC0_EMUX_R &= ~0x000F;
//    ADC0_SSMUX0_R &= ~0x000F;
//    ADC0_SSMUX0_R |= 0x0a;
//    ADC0_SSCTL0_R = 0x0006;
//    ADC0_IM_R &= ~ADC_SS_0;
//    ADC0_ACTSS_R |= ADC_SS_0;
//}
//uint16_t adc_read(void)
//{
//    ADC0_PSSI_R = 0x1; // conversion on ss3
//    while ((ADC0_RIS_R & ADC_SS_0) == 0)
//        ; // waiting for conversion to finish
//    uint16_t result = ADC0_SSFIFO0_R;  // read 12 bit result
//    ADC0_ISC_R = ADC_SS_0; // clears flag
//    return result;
//}
//
//float adc_dist(void)
//{
//    return ADC_DIST(adc_read());
//}
//
//uint16_t adc_read_average(void)
//{
//    int i;
//    uint32_t sum = 0; // stores total for all samples
//    for (i = 0; i < 16; i++)
//    { // takes multiple ADC readings
//        sum += adc_read(); // adds each sample to total
//    }
//    return sum / 16; // returns average val
//}
//
//// needs to average and distance
//float adc_dist_average(void)
//{
//    return ADC_DIST(adc_avg());
//}
