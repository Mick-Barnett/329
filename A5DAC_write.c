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

/*
/*
 * DAC_write.c
 *
 *  Created on: May 11, 2026
 *      Author: mickp
 */

/*
#include "DAC_write.h"

void DAC_write(uint16_t dac_data)
{
    uint16_t dac_word;

    /*
     * Keep only 12 data bits.
     */
    dac_data &= 0x0FFFU;

    /*
     * MCP4821 command:
     * bit 15 = 0
     * bit 14 = 0
     * bit 13 = 0, gain = 2x
     * bit 12 = 1, active mode
     * bits 11-0 = DAC data
     */
    dac_word = 0x1000U | dac_data;

    /*
     * CS low before SPI transfer.
     * PD14 is DAC CS.
     */
    GPIOD->BRR = GPIO_BRR_BR14;

    while ((SPI1->SR & SPI_SR_TXE) == 0U)
    {
    }

    *((volatile uint16_t *)&SPI1->DR) = dac_word;

    while ((SPI1->SR & SPI_SR_BSY) != 0U)
    {
    }

    /*
     * CS high after SPI transfer.
     */
    GPIOD->BSRR = GPIO_BSRR_BS14;
}
*/
