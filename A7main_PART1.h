/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  * project         : EE 329 S'26 A7
  * authors         : Joseph Matella & Mick Barnett
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "LPUART1.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
