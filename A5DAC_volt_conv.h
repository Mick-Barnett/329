#ifndef DAC_VOLT_CONV_H
#define DAC_VOLT_CONV_H

#include "stm32l4xx.h"
#include <stdint.h>

/*
 * ============================================================
 * DAC Voltage Conversion Function
 *
 * Converts a voltage in millivolts (mV) to a 12-bit DAC code.
 *
 * Input:
 *   voltage_mV : desired output voltage in millivolts
 *                (0 to 3300 mV)
 *
 * Output:
 *   12-bit DAC code (0 to 4095)
 *
 * Assumptions:
 *   - Using MCP4821 DAC
 *   - Internal VREF = 2.048 V
 *   - Gain = 2 → full-scale ≈ 4.096 V
 *
 * Formula:
 *   code = (voltage_mV * 4095) / 4096
 * ============================================================
 */

uint16_t DAC_volt_conv(uint16_t voltage_mV);

#endif
