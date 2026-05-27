/*
 * ADC.c
 *
 *  Created on: May 26, 2026
 *      Author: mickp
 */
#include "ADC.h"

volatile uint16_t ADC_result = 0;
volatile uint8_t ADC_ready = 0;
uint16_t adc_min = 0;
uint16_t adc_max = 0;
uint32_t adc_avg = 0;

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

void Process_ADC_samples(uint16_t array[], uint8_t length) {
	uint32_t sum = 0;

	adc_min = array[0];
	adc_max = array[0];

	for (uint8_t i = 0; i < length; i++) {
		// Parse through 
		if (array[i] < adc_min) {
			adc_min = array[i];
		}
		if (array[i] > adc_max) {
			adc_max = array[i];
		}
		sum += array[i];
	}
	adc_avg = sum / length;
}

