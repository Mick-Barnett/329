#ifndef SPI_INIT_H
#define SPI_INIT_H

#include "stm32l4xx.h"

/*
 * ============================================================
 * SPI1 CONFIGURATION FOR DAC
 *
 * Mode: SPI Mode 0
 * CPOL = 0
 * CPHA = 0
 *
 * Data size: 16-bit
 * MSB first
 *
 * Software NSS (CS handled manually using PD14)
 * ============================================================
 */

void SPI_init(void);

#endif
