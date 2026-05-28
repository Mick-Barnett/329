/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : main.c
 * @brief          : Src file for main.c ADC code
 * project         : EE 329 S'26 A8
 * authors         : Mick Barnett, Tyler Ragasa, Alan Odnoblyudov
 * version         : 0.1
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
uint16_t samples[NUM_SAMPLES];
uint8_t sample_index = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Private user code ---------------------------------------------------------*/
* function : PBSW_init()
* INs/OUTs : none
* action   : Configures GPIO (PC13) for pushbutton
*            - Enables GPIOC clock
*            - Sets PC13 as input (no pull-up/pull-down)
* authors  : Tyler Ragasa
* -------------------------------------------------------------------------- */
void PBSW_init(void)
{
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	// Enable GPIOC clk
   PBSW_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD13);	// No PUPD
   PBSW_PORT->MODER &= ~(GPIO_MODER_MODE13);	// Input mode
}
/* -----------------------------------------------------------------------------
* function : PBSW_is_pressed()
* INs      : none    OUTs: pressed = 1, released = 0.
* action   : Helper function for PBSW input. Active-low.
* authors  : Tyler Ragasa
* -------------------------------------------------------------------------- */
int PBSW_is_pressed(void) {return !(PBSW_PORT->IDR & PBSW_PIN);}
/* -----------------------------------------------------------------------------
* function : Relay_init()
* INs/OUTs : none
* action   : Configures GPIO (PC0) as output to drive relay
* authors  : Tyler Ragasa
* -------------------------------------------------------------------------- */
void Relay_init(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;				// Enable GPIOC clk
   RELAY_PORT->OTYPER &= ~GPIO_OTYPER_OT0;			// push-pull
   RELAY_PORT->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED0;	// low speed
   RELAY_PORT->PUPDR &= ~GPIO_PUPDR_PUPD0;			// no pull
	RELAY_PORT->MODER &= ~GPIO_MODER_MODE0;			// clear mode
   RELAY_PORT->MODER |=  (GPIO_MODER_MODE0_0);		// output mode
}

/* -----------------------------------------------------------------------------
* function : main
* INs/OUTs : none
* action   : When the button is pressed, turn on the GPIO out to allow current through
			 the BJT. Take samples of Ve in order to determine Icoil, by storing them in
			 a sample array and incrementing the index with each sample.
* authors  : Mick Barnett, Tyler Ragasa
* -------------------------------------------------------------------------- */
int main(void) {

	/* MCU Configuration--------------------------------------------------------*/
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	//initialize peripherals
	PBSW_init();
	Relay_init();
	ADC_init();

	
	while (1) {
		
		if (PBSW_is_pressed()) {
			RELAY_PORT->ODR &= ~RELAY_PIN;	// relay OFF
		}
		else {
			RELAY_PORT->ODR |= RELAY_PIN;	// relay ON
		}

		if (ADC_ready) {
			ADC_ready = 0; //reset ADC_ready flag

			samples[sample_index] = ADC_result; //fills the array with up to 20 results
			sample_index++; //increments the sample array index 
			
			//resets the indexing 
			if (sample_index >= NUM_SAMPLES) {
				Process_ADC_samples(samples, NUM_SAMPLES);
				sample_index = 0; 
			}

			ADC1->CR |= ADC_CR_ADSTART; //begin next conversion
		}
	}
}

//System Clock Configuration
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

//error Handler
void Error_Handler(void) {
	__disable_irq();
	while (1) {
	}
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif
