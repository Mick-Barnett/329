/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "ADC.h"

#define NUM_SAMPLES 20

// Global test variables
extern volatile uint16_t ADC_result;
extern volatile uint8_t ADC_ready;

extern uint16_t samples[NUM_SAMPLES];
extern uint8_t sample_index;
extern uint16_t adc_min;
extern uint16_t adc_max;
extern uint32_t adc_avg;

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
