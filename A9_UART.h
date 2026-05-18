/**
 ******************************************************************************
 * @file           : UART.h
 * @brief          : Header for main.c - A7 - UART
 * project         : EE 329 S'26 A7
 * authors         : Facundo Soto-Wang
 * version         : 0.1
 * date            : 2026-04-30
 * compiler        : STM32CubeIDE v.1.19.0
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 ******************************************************************************
 */
#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "delay.h"

#define BAUD_DIV (0x22B8)
#define UART_PORT GPIOG
#define CLEAR "\033[2J"
#define RESET "\033[2J\033[H"
#define HOME "\033[H"
#define BLINKING "\033[5m"
#define CLEAR_ATT "\033[0m"
#define RED "\e[31m"
#define GREEN "\e[32m"
#define BLUE "\e[34m"
#define WHITE "\033[0m]"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define SAVE "\033[7"
#define RET "\033[8"

extern uint8_t USER_FLAG;
extern volatile uint8_t Cursor_x;
extern volatile uint8_t Cursor_y;
extern volatile uint8_t red_timer;
enum command {
	up, down, forward, back
};

void UART1_INIT(void);
void LPUART_Print(const char* message);
void LPUART1_IRQHandler(void);
void LPUART_MOVE_CURSOR(int command, char* n);
void UART_Print_Char(char c);
void LPUART_SPLASH(void);
void BOUNDS(void);
void DEL_CHAR(void);
void PRINT_IN_PLACE(const char* a);
#endif /* INC_UART_H_ */


