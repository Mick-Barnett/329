
#include "ADC.h"

// results + per-batch min/max/avg in counts and mV
volatile uint16_t ADC_result = 0;
volatile uint8_t ADC_ready = 0;
uint16_t adc_min_count = 0;
uint16_t adc_max_count = 0;
uint16_t adc_avg_count = 0;
uint16_t adc_min_mV = 0;
uint16_t adc_max_mV = 0;
uint16_t adc_avg_mV = 0;

// calibration from the regression (counts -> voltage)
#define CAL_SLOPE_NUM 12341UL
#define CAL_SLOPE_DEN 10000UL
#define CAL_OFFSET 6UL

// emitter sense resistor R_E in milliOhms (TODO: set to DMM-measured value)
#define R_E_MOHM 1000UL

// initialize ADC1: PA0/ch5 single-ended, 12-bit, single conv, EOC interrupt
void ADC_init(void) {
    // turn on ADC clock (HCLK/1) and power up the analog regulator
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos);
    ADC1->CR &= ~(ADC_CR_DEEPPWD);
    ADC1->CR |= (ADC_CR_ADVREGEN);
    delay_us(20);

    // single-ended calibration (ADC must be disabled)
    ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);
    ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL) {
        ;
    }

    // enable ADC and wait until ready
    ADC1->ISR |= (ADC_ISR_ADRDY);
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
        ;
    }
    ADC1->ISR |= (ADC_ISR_ADRDY);

    // 1 conversion on ch 5, single-conv mode, s/w trigger, 12-bit
    ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);
    ADC1->SMPR1 |= (SAMPLE_TIME << ADC_SMPR1_SMP5_Pos);
    ADC1->CFGR &= ~( ADC_CFGR_CONT | ADC_CFGR_EXTEN | ADC_CFGR_RES );

    // enable end-of-conversion interrupt
    ADC1->IER |= ADC_IER_EOCIE;
    ADC1->ISR |= ADC_ISR_EOC;
    NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F));
    __enable_irq();

    // PA0 to analog mode (set MODER last), then start first conversion
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
    GPIOA->MODER |= (GPIO_MODER_MODE0);
    ADC1->CR |= ADC_CR_ADSTART;
}

// EOC interrupt: latch the result and flag main (reading DR clears EOC)
void ADC1_2_IRQHandler(void) {
    if (ADC_ISR_EOC & ADC1->ISR) {
        ADC_result = ADC1->DR;
        ADC_ready = 1;
        ADC1->ISR = ADC_ISR_EOC;
    }
}

// calibrated count -> millivolts (integer)
uint16_t ADC_count_to_mV(uint16_t count) {
    if (count <= CAL_OFFSET) return 0;
    uint32_t mV = (((uint32_t)count - CAL_OFFSET) * CAL_SLOPE_DEN) / CAL_SLOPE_NUM;
    return (uint16_t)mV;
}

// calibrated count -> microvolts (x1000 of mV; 64-bit math avoids overflow)
uint32_t ADC_count_to_uV(uint16_t count) {
    if (count <= CAL_OFFSET) return 0;
    uint64_t uV = ((uint64_t)(count - CAL_OFFSET) * CAL_SLOPE_DEN * 1000UL) / CAL_SLOPE_NUM;
    return (uint32_t)uV;
}

// coil current estimate: I[mA] = V_RE[uV] / R_E[mOhm]
// note: PA0 reads the emitter node, so this is really I_E (= I_C + I_B)
uint16_t ADC_coil_current_mA(uint16_t count) {
    return (uint16_t)(ADC_count_to_uV(count) / R_E_MOHM);
}

// scan the batch for min/max/avg counts, then convert each to mV
void Process_ADC_samples(uint16_t array[], uint8_t length) {
    uint32_t sum = 0;
    adc_min_count = array[0];
    adc_max_count = array[0];
    for (uint8_t i = 0; i < length; i++) {
        if (array[i] < adc_min_count) {
            adc_min_count = array[i];
        }
        if (array[i] > adc_max_count) {
            adc_max_count = array[i];
        }
        sum += array[i];
    }
    adc_avg_count = (uint16_t)(sum / length);
    adc_min_mV = ADC_count_to_mV(adc_min_count);
    adc_max_mV = ADC_count_to_mV(adc_max_count);
    adc_avg_mV = ADC_count_to_mV(adc_avg_count);
}
