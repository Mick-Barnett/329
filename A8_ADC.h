/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : ADC.h
 * @brief          : header file for ADC.c
 * project         : EE 329 S'26 A5
 * authors         : Tyler Ragasa, Mick Barnett, Alan Odnoblyudov
 * version         : 0.1
 * date            : 5/26/26
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 * WIRING
 *
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
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
