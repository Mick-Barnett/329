/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : RNG.c
 * @brief          : Inits RNG feature, functions to generate random numbers
 * project         : EE 329 S'26 A9
 * authors         : Facundo Soto-Wang
 * version         : 0.1
 * date            : 230413
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5*/

 #include "RNG.h"

void RNG_Init(void)
    // enable RNG peripheral clock
    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    // enable HSI48
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY));
    // select HSI48 as RNG clock source (FIXED FIELD)
    RCC->CCIPR &= ~RCC_CCIPR_RNGSEL;
    RCC->CCIPR |= RCC_CCIPR_RNGSEL_1;  // HSI48
    // configure RNG
    RNG->CR &= ~RNG_CR_IE;
    RNG->CR &= ~RNG_CR_CED;
    RNG->CR |= RNG_CR_RNGEN;
}

uint32_t RNG_num(void){
	while (!(RNG->SR & RNG_SR_DRDY));  //wait for data to be ready
	return RNG->DR;					     //return random data
}
