
#ifndef INC_ADC_H_
#define INC_ADC_H_
#include "stm32l4xx_hal.h"
#include "delay.h"
#include <stdint.h>

// ch 5 sample time: 1 = 6.5 clk, 4 = 47.5 clk, 7 = 640.5 clk
#define SAMPLE_TIME 1

// shared globals (results + per-batch min/max/avg in counts and mV)
extern volatile uint16_t ADC_result;
extern volatile uint8_t ADC_ready;
extern uint16_t adc_min_count;
extern uint16_t adc_max_count;
extern uint16_t adc_avg_count;
extern uint16_t adc_min_mV;
extern uint16_t adc_max_mV;
extern uint16_t adc_avg_mV;

// function prototypes
void ADC_init(void);
void ADC1_2_IRQHandler(void);
uint16_t ADC_count_to_mV(uint16_t count);
uint32_t ADC_count_to_uV(uint16_t count);
uint16_t ADC_coil_current_mA(uint16_t count);
void Process_ADC_samples(uint16_t array[], uint8_t length);

#endif
