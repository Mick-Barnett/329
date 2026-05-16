/*
 *******************************************************************************
 * EE 329 A5 NUCLEO SPI & DAC INTERFACE
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
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5

 /* USER configure GPIO pins for I2C alternate functions SCL and SDA */
// Configure I2C
void I2C_Init(void) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; // enable I2C bus clock
	I2C1->CR1 &= ~(I2C_CR1_PE); // put I2C into reset (release SDA, SCL)
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF); // filters: enable analog
	I2C1->CR1 &= ~(I2C_CR1_DNF); // filters: disable digital
	I2C1->TIMINGR = 0x00303D5B; // 16 MHz SYSCLK timing from CubeMX
	I2C1->CR2 |= (I2C_CR2_AUTOEND); // auto send STOP after transmission
	I2C1->CR2 &= ~(I2C_CR2_ADD10); // 7-bit address mode
	I2C1->CR1 |= (I2C_CR1_PE); // enable I2C
}

void I2C_Write(void) {
// build EEPROM transaction
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN); // set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES); // clear Byte count
	I2C1->CR2 |= (3 << I2C_CR2_NBYTES_Pos); // write 3 bytes (2 addr, 1 data)
	I2C1->CR2 &= ~(I2C_CR2_SADD); // clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1)); // device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START; // start I2C WRITE op
	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while (!(I2C1->ISR & I2C_ISR_TXIS))
		;  // wait for start condition to transmit
	I2C1->TXDR = (EEPROM_MEMORY_ADDR >> 8);
	// xmit MSByte of address
	/* address high, address low, data  -  wait at least 5 ms before READ
	 the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
	I2C1->TXDR = (data); //send data
}

void I2C_Read(void) {
	// build EEPROM transaction
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN); // set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES); // clear Byte count
	I2C1->CR2 |= (3 << I2C_CR2_NBYTES_Pos); // write 3 bytes (2 addr, 1 data)
	I2C1->CR2 &= ~(I2C_CR2_SADD); // clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos + 1)); // device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START; // start I2C WRITE op
	/* USER wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while (!(I2C1->ISR & I2C_ISR_TXIS))
		;  // wait for start condition to transmit
	I2C1->TXDR = (EEPROM_MEMORY_ADDR >> 8);
	// xmit MSByte of address
	/* address high, address low, data  -  wait at least 5 ms before READ
	 the READ op has new NBYTES (WRITE 2 then READ 1) & new RD_WRN for 3rd Byte */
	I2C1->TXDR = (data); //send data
}

