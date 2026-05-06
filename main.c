/*
 *******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * project         : EE 329 S'26 A7
 * authors         : Joseph Matella & Mick Barnett
 * version         : 0.1
 * date            : May 4, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: Flow control for splash screen and game rendering
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | N/A
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *  0.1 | 5/6/2026 | Initial flow control for game rendering
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "LPUART1.h"
#include "delay.h"

// Define flag for ISR to indicate game character position needs to be updated
volatile bool update_pos_flag = 1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // Initialization functions
  LPUART1_init();
  SysTick_Init();

  // Echo Character portion of lab
  // Written over by game interface
  LPUART_ESC_Print("2J");    // clear screen
  LPUART_ESC_Print("H");     // move cursor to top left

  LPUART_ESC_Print("3B");           // move cursor down 3 lines
  LPUART_ESC_Print("5C");           // move cursor right 5 spaces
  LPUART_Print("All good students read the");
  LPUART_ESC_Print("1B");           // move cursor down 1 line
  LPUART_ESC_Print("21D");          // move cursor left 21 spaces
  LPUART_ESC_Print("5m");           // blinking text on
  LPUART_Print("Reference Manual");
  LPUART_ESC_Print("H");            // cursor to top left
  LPUART_ESC_Print("0m");           // remove attributes (blink off)
  LPUART_Print("Input: ");

  // Rudimentary game face portion of lab
  LPUART_ESC_Print("2J");    // clear screen

  // Render the splash screen
  Render_SplashScreen();
  // Delay 5 seconds
  delay_us(50000000);
  // Clear the screen
  LPUART_ESC_Print("2J");
  // Render the game border
  Render_Border();

  while (1)
  {
	  // Check if awsd has been pressed to move the game character
	  if (update_pos_flag) {
		  // Update the game character position based on key press
		  Update_Character_Position();
		  // Render the game character at the new position
		  Render_Character();
	  }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
