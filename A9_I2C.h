/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : I2C.h
 * @brief          : header file for I2C.c
 * project         : EE 329 S'26 A5
 * authors         : Facundo Soto-Wang & Mick Barnett
 * version         : 0.1
 * date            : 230413
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
#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32l4xx_hal.h"

#define EEPROM_ADDRESS 0b1010110 //address of EEPROM Device (not memory address)

uint8_t EEPROM_Read(uint16_t EEPROM_MEMORY_ADDR);
void EEPROM_write(uint16_t EEPROM_MEMORY_ADDR, uint8_t data);
void I2C_Init(void);

#endif /* INC_I2C_H_ */
