
#include "stm32l4xx_hal.h"
#include "delay.h"
#include <stdio.h>
typedef int bool;

// terminal dimensions
#define ROWS 24
#define COLS 80

// function prototypes
void LPUART1_init(void);
void LPUART_Print(const char* message);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char* command);
void LPUART_ESC_Print_Pos(uint8_t row, uint8_t col);

// A8: live ADC table (step 3) + coil current (step 10b)
void ADC_Render(uint16_t min_cnt, uint16_t max_cnt, uint16_t avg_cnt,
                uint16_t min_mV,  uint16_t max_mV,  uint16_t avg_mV,
                uint16_t coil_mA);
