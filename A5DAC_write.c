/*
 * DAC_write.c
 *
 *  Created on: May 11, 2026
 *      Author: mickp
 */


void DAC_write(uint16_t dac_data) {
	uint16_t dac_word;

	dac_data &= 0x0FFF;
	dac_word = 0x1000 | dac_data;

	while (!(SPI1->SR & SPI_SR_TXE));

	*((volatile uint16_t *)&SPI1->DR) = dac_word;

	while (SPI1->SR & SPI_SR_BSY);
}
