#ifndef DAC_WRITE_H
#define DAC_WRITE_H

#include "stm32l4xx.h"
#include <stdint.h>

/*
 * Writes one 12-bit DAC data value to the MCP4821 over SPI1.
 *
 * Pin mapping:
 * PA5  -> SPI1_SCK
 * PA7  -> SPI1_MOSI / SDI
 * PD14 -> DAC CS
 *
 * Assumes:
 * - SPI1 has already been initialized
 * - PD14 has already been configured as GPIO output
 */
void DAC_write(uint16_t dac_data);

#endif
