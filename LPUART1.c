/*
 *******************************************************************************
 * @file           : LPUART1.c
 * @brief          : X
 * project         : EE 329 S'26 AX
 * authors         : joeym
 * version         : 0.1
 * date            : May 4, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: X
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | LCD - DB4 - 11 | PC0             | CN9-3              | OUT
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *      |          | 
 *******************************************************************************
 *
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

//configures LPUART1
#include "LPUART1.h"

void LPUART1_init(void) {
	// PG7 = LPUART1_TX, PG8 = LPUART1_RX, AF8 to ST-LINK VCP

	PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	GPIOG->MODER &= ~((3U << (2 * 7)) | (3U << (2 * 8)));
	GPIOG->MODER |=  ((2U << (2 * 7)) | (2U << (2 * 8)));  // alternate function

	GPIOG->AFR[0] &= ~(0xFU << (4 * 7));
	GPIOG->AFR[0] |=  (8U   << (4 * 7));                     // PG7 AF8

	GPIOG->AFR[1] &= ~(0xFU << (4 * (0)));
	GPIOG->AFR[1] |=  (8U   << (4 * (0)));               // PG8 AF8

	GPIOG->OTYPER &= ~((1U << 7) | (1U << 8));             // push-pull
	GPIOG->PUPDR  &= ~((3U << (2 * 7)) | (3U << (2 * 8))); // no pull
	GPIOG->OSPEEDR |= ((3U << (2 * 7)) | (3U << (2 * 8))); // high speed

	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
	/* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
		select AF mode and specify which function with AFR[0] and AFR[1] */
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
	/* USER: set baud rate register (LPUART1->BRR) */
	LPUART1->BRR = 0x22B9;
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                   // enable global interrupts
}

//character transmission for a string message
void LPUART_Print( const char* message ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
         ;
      LPUART1->TDR = message[iStrIdx];       // send this character
	   iStrIdx++;                             // advance index to next char
   }
}

//character transmission for a string message
void LPUART_ESC_Print( const char* command ) {
   uint16_t iStrIdx = 0;
   while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
            ;
   LPUART1->TDR = 0x1B;       // send ESC

	while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
				;
   LPUART1->TDR = '[';       // send [

   // Send comand
   while ( command[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
         ;
      LPUART1->TDR = command[iStrIdx];       // send this character
	   iStrIdx++;                             // advance index to next char
   }
}
////////////////////////////////////////
//Needs to be similar to LPUART pring but include a case for esc
//LPUART_ESC_Print(){}
///////////////////////////////////////

//
void LPUART1_IRQHandler( void  ) {
   uint8_t charRecv;
   if (LPUART1->ISR & USART_ISR_RXNE) {
      charRecv = LPUART1->RDR;
      switch ( charRecv ) {
	   case 'R':
         LPUART_Print("joe");
			/* USER: process R to ESCape code back to terminal */
	      break;
         /* USER : handle other ESCape code cases */
	   default:
	      while( !(LPUART1->ISR & USART_ISR_TXE) )
               ;    // wait for empty TX buffer
		LPUART1->TDR = charRecv;  // echo char to terminal
	}  // end switch
   }
}



