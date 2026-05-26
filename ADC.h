/*
 * ADC.h
 *
 *  Created on: May 26, 2026
 *      Author: mickp
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>

extern volatile uint16_t ADC_result;
extern volatile uint8_t ADC_ready;

void ADC_init(void);
void ADC1_2_IRQHandler(void);
void Process_ADC_samples(uint16_t array[], uint8_t length);

#endif

