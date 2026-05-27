/*
 * ADC.h
 *
 *  Created on: May 26, 2026
 *      Author: mickp
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"
#include "delay.h"
#include <stdint.h>

// ch 5 Sample Time: 1 = 6.5 clocks, 4 = 47.5 clocks, 7 = 640.5 clocks
#define SAMPLE_TIME 1 

// global test variables
extern volatile uint16_t ADC_result;
extern volatile uint8_t ADC_ready;
extern uint16_t adc_min;
extern uint16_t adc_max;
extern uint32_t adc_avg;

void ADC_init(void);
void ADC1_2_IRQHandler(void);
void Process_ADC_samples(uint16_t array[], uint8_t length);

#endif

