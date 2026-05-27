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

// ADC State Variables
extern volatile uint16_t ADC_result;
extern volatile uint8_t ADC_ready;
// ADC Result Variables
extern uint16_t adc_min_count;
extern uint16_t adc_max_count;
extern uint16_t adc_avg_count;
extern uint16_t adc_min_mV;
extern uint16_t adc_max_mV;
extern uint16_t adc_avg_mV;

void ADC_init(void);
void ADC1_2_IRQHandler(void);
void Process_ADC_samples(uint16_t array[], uint8_t length);
uint16_t ADC_count_to_mV(uint16_t count);

#endif

