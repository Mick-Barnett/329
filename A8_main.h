/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "ADC.h"

#define NUM_SAMPLES 20
#define RELAY_PORT GPIOC
#define RELAY_PIN GPIO_PIN_0
#define PBSW_PORT GPIOC
#define PBSW_PIN GPIO_PIN_13

extern uint16_t samples[NUM_SAMPLES];
extern uint8_t sample_index;

void PBSW_init(void);
int PBSW_is_pressed(void);
void Relay_init(void);

void Error_Handler(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H *//* USER CODE END Includes */