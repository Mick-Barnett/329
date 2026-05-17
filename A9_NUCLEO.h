/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : NUCLEO.h
 * @brief          : header file for NUCLEO.c
 * project         : EE 329 S'26 A5
 * authors         : Facundo Soto-Wang
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
#ifndef INC_NUCLEO_H_
#define INC_NUCLEO_H_

#define LED_PIN 0x80 //mask for PB7 (LD2 PIN)

void LD2_Init(void);
void LED_on(void);
void LED_off(void);


#endif /* INC_NUCLEO_H_ */
