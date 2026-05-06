/**
  ******************************************************************************
  * @file           : LPUART1.h
  * @brief          : Header for LPUART1.c file.
  * project         : EE 329 S'26 A7
  * authors         : Joseph Matella & Mick Barnett
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "stm32l4xx_hal.h"
#include "delay.h"
#include <stdio.h>

typedef int bool;

// Rows and columns of splash screen
#define ROWS 24
#define COLS 80

// Game border defines to account for offset from edge
#define BORDER_OFFSET 4
#define BORDER_ROW_MAX (ROWS - BORDER_OFFSET)
#define BORDER_ROW_MIN (BORDER_OFFSET + 1)
#define BORDER_COL_MAX (COLS - BORDER_OFFSET)
#define BORDER_COL_MIN (BORDER_OFFSET + 1)

// Function declarations
void LPUART1_init(void);
void LPUART_Print(const char* message);
void LPUART1_IRQHandler(void);
void LPUART_ESC_Print(const char* command);
void LPUART_ESC_Print_Pos(uint8_t row, uint8_t col);
void Render_SplashScreen(void);
void Render_Border(void);
void Update_Character_Position(void);
void Render_Character(void);

