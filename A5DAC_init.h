#ifndef DAC_INIT_H
#define DAC_INIT_H

#include "stm32l4xx.h"
#include <stdint.h>

/*
 * ============================================================
 * DAC MODULE (MCP4821)
 *
 * Pin Mapping:
 * PA5  -> SPI1_SCK  -> DAC SCK
 * PA7  -> SPI1_MOSI -> DAC SDI
 * PD14 -> GPIO      -> DAC CS (MANUAL CHIP SELECT)
 *
 * IMPORTANT:
 * PD14 is NOT SPI alternate function.
 * It is controlled manually in DAC_write().
 *
 * MCP4821 connections:
 * VDD  -> 3.3V
 * VSS  -> GND
 * LDAC -> GND
 * SHDN -> 3.3V
 * VOUT -> analog output
 * ============================================================
 */

#define DAC_MAX_MV      3300U
#define DAC_MAX_CODE    4095U

/* MCP4821 control bits (0x1000 = active + gain=2) */
#define DAC_CTRL_BITS   0x1000U

void DAC_init(void);
uint16_t DAC_volt_conv(uint16_t millivolts);
void DAC_write(uint16_t dac_code);
void DAC_set_mV(uint16_t millivolts);

#endif
