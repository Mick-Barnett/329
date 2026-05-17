/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : RNG.h
 * @brief          : header file for RNG.c
 * project         : EE 329 S'26 A5
 * authors         : Facundo Soto-Wang
 * version         : 0.1
 * date            : 230413
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#ifndef INC_RNG_H_
#define INC_RNG_H_

#include "stm32l4xx_hal.h"

uint32_t RNG_num(void);
void RNG_Init(void);

#endif /* INC_RNG_H_ */
