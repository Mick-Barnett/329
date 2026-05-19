/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c - A9 - I2C
 * project         : EE 329 S'26 A4
 * authors         : Facundo Soto-Wang & Mick Barnett
 * version         : 0.1
 * date            : 2026-04-30
 * compiler        : STM32CubeIDE v.1.19.0
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2 & APB1
 ******************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l4xx_hal.h"
#include "I2C.h"
#include "delay.h"
#include "RNG.h"
#include "NUCLEO.h"


// global test variables
extern uint8_t random_data;
extern uint16_t random_address;
extern uint8_t mem_data;


void Error_Handler(void);
void SystemClock_Config(void);

#endif
