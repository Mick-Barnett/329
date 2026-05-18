/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : NUCLEO.c
 * @brief          : Inits on board LED (LD2), contains function to toggle LED
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

 #include "NUCLEO.h"

/*-----------------------------------------------------------------------------
 * function : LD2_Init();
 * INs      : none
 * OUTs     : none
 * action   : configure PB7 as push pull out for LD2
 * authors  : Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void LD2_Init(void){
 RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;
 GPIOB->MODER  &= ~(GPIO_MODER_MODE7);
 GPIOB->MODER  |=  (GPIO_MODER_MODE7_0);
 GPIOB->OTYPER &= ~(GPIO_OTYPER_OT7);
 GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD7);
 GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7);
 GPIOB->BRR     = LED_PIN;
}

/*-----------------------------------------------------------------------------
 * function : LED_on();
 * INs      : none
 * OUTs     : none
 * action   : turns LD2 on
 * authors  : Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void LED_on(void){
 GPIOB->BSRR = LED_PIN;
}

/*-----------------------------------------------------------------------------
 * function : LED_off();
 * INs      : none
 * OUTs     : none
 * action   : turn LD2 off
 * authors  : Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void LED_off(void){
   GPIOB->BRR = LED_PIN;
}

/*-----------------------------------------------------------------------------
 * function : PBSW_GPIO_Init();
 * INs      : none
 * OUTs     : none
 * action   : configures on-board pushbutton switch as input 
 * authors  : Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void PBSW_GPIO_Init(void){
 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
 GPIOC->MODER &= ~(GPIO_MODER_MODE13);
 GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);
}

/*-----------------------------------------------------------------------------
 * function : PBSW_IsPressed();
 * INs      : none
 * OUTs     : none
 * action   : returns 1 if B1 Iis pressed and 0 otherwise
 * authors  : Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
int PBSW_IsPressed(void){
 return ((PBSW_PORT->IDR >> PBSW_PIN_NUM) & 1U);
}
