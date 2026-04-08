#ifndef ADC_H_
#define ADC_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#define ADC_MODULE_0 1
#define ADC_MODULE_1 2

#define ADC_SS_0 1
#define ADC_SS_1 2
#define ADC_SS_2 4
#define ADC_SS_3 8

#define ADC_DIST(x) (7314935.71 * pow(x, -1.7318))
#ifndef adc_avg
#define adc_avg adc_read_average
#endif
#ifndef adc_avgd
#define adc_avgd adc_dist_average
#endif

void adc_init(void);
uint16_t adc_read(void);
float adc_dist(void);
uint16_t adc_read_average(void);
float adc_dist_average(void);

#endif
