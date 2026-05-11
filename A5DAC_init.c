/*
 * DAC_init.c
 *
 *  Created on: May 11, 2026
 *      Author: mickp
 */
#include "SPI_init.h"

//initialize the SPI peripheral to communicate with the DAC
void DAC_init(void) {
	// enable clock for GPIOA, GPIOD & SPI1
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIODEN);
	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);

	// configure PA5, PA7 for SPI1 AF5
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE7);
	GPIOA->MODER |=  (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);

	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT7);

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD7);

	GPIOA->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED5_Pos) |
	                   (3 << GPIO_OSPEEDR_OSPEED7_Pos));

	GPIOA->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL5_Pos) |
	                   (0xF << GPIO_AFRL_AFSEL7_Pos));

	GPIOA->AFR[0] |=  ((0x5 << GPIO_AFRL_AFSEL5_Pos) |
	                   (0x5 << GPIO_AFRL_AFSEL7_Pos));

	// configure PD14 for SPI1 AF5
	GPIOD->MODER &= ~(GPIO_MODER_MODE14);
	GPIOD->MODER |=  (GPIO_MODER_MODE14_1);

	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT14);

	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD14);

	GPIOD->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED14_Pos);

	GPIOD->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL14_Pos);
	GPIOD->AFR[1] |=  (0x5 << GPIO_AFRH_AFSEL14_Pos);

	SPI_init();
}
