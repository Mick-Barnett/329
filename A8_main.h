/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : main.h
 * @brief          : header file for main.c
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

