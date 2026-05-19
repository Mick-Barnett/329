/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : I2C.c
 * @brief          : Inits I2C1 and contains read/write functions
 * project         : EE 329 S'26 A9
 * authors         : Facundo Soto-Wang & Mick Barnett
 * version         : 0.1
 * date            : 230413
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 * WIRING
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5*/

#include "I2C.h"
#include "delay.h"

/*-----------------------------------------------------------------------------
 * function : I2C_Init();
 * INs      : none
 * OUTs     : none
 * action   : initializes I2C1 for fast speed, 4MHz APB1,
 *            auto stop and 7-bit addresses
 * authors  : EE329 Lab Manual - A9 sample code
 *            Modified by Mick Barnett & Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void I2C_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; // enable I2C bus clock
	//pin configuration for PB8, PB9
	GPIOB -> MODER   &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB -> MODER   |=  (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);//AF MODE
	GPIOB -> OTYPER  |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);//open drain
	GPIOB -> PUPDR   &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9); //no PUPD
	GPIOB -> OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED8_Pos) |
								(3 << GPIO_OSPEEDR_OSPEED9_Pos)); //VF Speed

	I2C1->CR1 &= ~(I2C_CR1_PE); // put I2C into reset (release SDA, SCL)
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF); // filters: enable analog
	I2C1->CR1 &= ~(I2C_CR1_DNF); // filters: disable digital
	I2C1->TIMINGR = 0x00100002; // 16 MHz SYSCLK timing from CubeMX
	I2C1->CR2 |= (I2C_CR2_AUTOEND); // auto send STOP after transmission
	I2C1->CR2 &= ~(I2C_CR2_ADD10); // 7-bit address mode
	GPIOB ->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL9_Pos);
	GPIOB ->AFR[1] |=  (0x0004 << GPIO_AFRH_AFSEL9_Pos);
	GPIOB ->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos);
	GPIOB ->AFR[1] |=  (0x0004 << GPIO_AFRH_AFSEL8_Pos);
	I2C1->CR1 |= (I2C_CR1_PE); // enable I2C
}
/*-----------------------------------------------------------------------------
 * function : EEPROM_write();
 * INs      : 2 byte memory address
 * OUTs     : none
 * action   : Sets I2C1 to write mode
 *            then writes 2 byte memory address after sending slave address
 *            followed by data byte to be stored in memory
 * authors  : EE329 Lab Manual - A9 sample code - Modified by Mick Barnett
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
void EEPROM_write(uint16_t EEPROM_MEMORY_ADDR, uint8_t data) {
// build EEPROM transaction
	I2C1->CR1 &= ~(I2C_CR1_PE); // release I2C
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN); // set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES); // clear Byte count
	I2C1->CR2 |= (3 << I2C_CR2_NBYTES_Pos); // write 3 bytes (2 addr, 1 data)
	I2C1->CR2 &= ~(I2C_CR2_SADD); // clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1)); // device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START; // start I2C WRITE op
	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while (!(I2C1->ISR & I2C_ISR_TXIS))
		;  // wait for start condition to transmit
	I2C1->TXDR = (EEPROM_MEMORY_ADDR >> 8);	// xmit MSByte of address

	/* address high, address low, data  -  wait at least 5 ms before READ
	 the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
	///////////////
	// xmit LSByte of address
	while (!(I2C1->ISR & I2C_ISR_TXIS));

	I2C1->TXDR = (EEPROM_MEMORY_ADDR & 0xFF);

	// wait before sending next byte
	while (!(I2C1->ISR & I2C_ISR_TXIS));

	I2C1->TXDR = (data); //send data

	// wait for AUTOEND-generated STOP
	while (!(I2C1->ISR & I2C_ISR_STOPF));

	// clear STOP flag
	I2C1->ICR |= I2C_ICR_STOPCF;

	// EEPROM internal write cycle time - 5ms
	delay_us(5000);
}
/*-----------------------------------------------------------------------------
 * function : EEPROM_Read();
 * INs      : 2 byte memory address
 * OUTs     : data byte from memory address
 * action   : Sets I2C1 to write mode
 *            then writes 2 byte memory address after sending slave address
 *            Sets I2C1 to read mode, then reads the data from EEPROM
 *            EEPROM data byte is returned by function
 * authors  : EE329 Lab Manual - A9 sample code - Modified by Facundo Soto-Wang
 * version  : 0.1
 * date     : 260517
 * usage    : called by main.c
 *----------------------------------------------------------------------------*/
uint8_t EEPROM_Read(uint16_t EEPROM_MEMORY_ADDR) {
	// build EEPROM transaction
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN); // set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES); // clear Byte count
	I2C1->CR2 |= (2 << I2C_CR2_NBYTES_Pos); // write 2 bytes (2 addr)
	I2C1->CR2 &= ~(I2C_CR2_SADD); // clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1)); // device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START; // start I2C write op
	while (!(I2C1->ISR & I2C_ISR_TXIS));// wait for start condition to transmit
	I2C1->TXDR = (EEPROM_MEMORY_ADDR >> 8); // xmit MSByte of address
	while (!(I2C1->ISR & I2C_ISR_TXIS)); // wait before sending next byte
	I2C1->TXDR = (uint8_t)(EEPROM_MEMORY_ADDR & 0xFF);//xmit LSByte of address
	I2C1->CR1 &= ~(I2C_CR1_PE); // release I2C
   delay_us(5000);
	I2C1->CR1 |= (I2C_CR1_PE); // enable I2C
   I2C1->CR2 |= (I2C_CR2_RD_WRN); // set read mode
   I2C1->CR2 &= ~(I2C_CR2_NBYTES); // clear Byte count
	I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos); // read 1 byte of data
   I2C1->CR2 |= I2C_CR2_START; // start I2C read op
	while (!(I2C1->ISR & I2C_ISR_RXNE));  //wait until data is received
   uint8_t mem_data = (I2C1->RXDR);      //read data from I2C receive buffer
	while (!(I2C1->ISR & I2C_ISR_STOPF)); // wait for AUTOEND-generated STOP
	I2C1->ICR |= I2C_ICR_STOPCF;			  // clear STOP flag
	return(mem_data);
	I2C1->CR1 &= ~(I2C_CR1_PE); // release I2C
}
