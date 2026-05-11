#include "DAC_volt_conv.h"

uint16_t DAC_volt_conv(uint16_t voltage_mV) {

    if (voltage_mV > 3300) {
        voltage_mV = 3300;
    }

    uint32_t temp = (uint32_t)voltage_mV * 4095;
    temp = temp / 4096;

    return (uint16_t)temp;
}
