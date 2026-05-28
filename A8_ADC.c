/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : ADC.c
 * @brief          : Src file for ADC.c code
 * project         : EE 329 S'26 A8
 * authors         : Mick Barnett, Tyler Ragasa, Alan Odnoblyudov
 * version         : 0.3
 * date            : 05/26/2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 24 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PIN ASSIGNMENTS
 *PC0 --> GPIO Out
 *PA0 --> Analog Input
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */
#include "ADC.h"

// Initialize ADC parameters
volatile uint16_t ADC_result = 0;
volatile uint8_t ADC_ready = 0;
uint16_t adc_min_count = 0;
uint16_t adc_max_count = 0;
uint16_t adc_avg_count = 0;
uint16_t adc_min_mV = 0;
uint16_t adc_max_mV = 0;
uint16_t adc_avg_mV = 0;
// Calibration Constants
#define CAL_SLOPE_NUM 12341UL
#define CAL_SLOPE_DEN 10000UL
#define CAL_OFFSET 6UL


void ADC_init(void) {
//run the ADC with a clock of at least 24Mhz
//single channel,single conversion software tirggered by ADC_CR_ADSTART bit
//12-bit resolution, 3.3V ful scale range, 6.5 clock sample time
//use GPIO pin configured for analog input, interrupt on end-of-conversion

	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);          // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {
		;
	}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
		;
	}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (SAMPLE_TIME << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time
	ADC1->CFGR &= ~( ADC_CFGR_CONT |         // single conversion mode
			ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
			ADC_CFGR_RES);        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR |= ADC_ISR_EOC;                  // set to clear EOC flag
	NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->MODER |= (GPIO_MODER_MODE0);   // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion
}

void ADC1_2_IRQHandler(void) {
//verify that the ADC_ISR_EOC flag is the IRQ trigger
//copy the ADC conversion result from ADC1->DR to a global variable
//set a global flag to tell the main loop that a conversion is ready
	if (ADC_ISR_EOC & ADC1->ISR) {
		ADC_result = ADC1->DR;
		ADC_ready = 1;
		ADC1->ISR = ADC_ISR_EOC;
	}
}

//converts to mV by subtracting known offset and dividing by conversion rate defined in ADC.h
uint16_t ADC_count_to_mV(uint16_t count) {
	if(count<=CAL_OFFSET) return 0;
	uint32_t mV = (((uint32_t)count - CAL_OFFSET)*CAL_SLOPE_DEN)/CAL_SLOPE_NUM;
	return (uint16_t)mV;
}


void Process_ADC_samples(uint16_t array[], uint8_t length) {
//intializes sum at 0	
	uint32_t sum = 0;
//intializes adc min and max as the 0 index to start
	adc_min_count = array[0];
	adc_max_count = array[0];

	//increments through the indexs with each sample to find min/max
	for (uint8_t i = 0; i < length; i++) {
		//compares each sample to known min, if sample is lower, it becomes the new min
		if (array[i] < adc_min_count) {
			adc_min_count = array[i];
		}
		//compares each sample to known max, if sample is higher, it becomes the new max
		if (array[i] > adc_max_count) {
			adc_max_count = array[i];
		}
		//adds each new sample to the sum
		sum += array[i];
	}
	//takes average
	adc_avg_count = (uint16_t)(sum / length);
	//converts to mV
	adc_min_mV = ADC_count_to_mV(adc_min_count);
	adc_max_mV = ADC_count_to_mV(adc_max_count);
	adc_avg_mV = ADC_count_to_mV(adc_avg_count);
}
