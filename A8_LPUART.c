/*
 *******************************************************************************
 * @file           : LPUART1.c
 * @brief          : LPUART and Render
 * project         : EE 329 S'26 A7
 * authors         : Joseph Matella & Mick Barnett
 * version         : 0.1
 * date            : May 4, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: Handles sending and receiving messages across LPUART
 * communication protocol. Includes functions for rendering game elements
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | N/A
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *  0.1 | 5/6/2026 | Creation of LPUART and Render functions
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#include "LPUART1.h"

extern volatile bool update_pos_flag;
volatile uint8_t last_key = 0;
uint8_t char_row_pos      = ROWS / 2;
uint8_t char_col_pos      = COLS / 2;
uint8_t prev_char_row_pos = ROWS / 2;
uint8_t prev_char_col_pos = COLS / 2;

/* -----------------------------------------------------------------------------
 * function : LPUART1_init()
 * INs      : none
 * OUTs     : none
 * action   : Initiate LPUART1
 * Citation : adapted from [Lab Manual] pg 30
 * -------------------------------------------------------------------------- */
void LPUART1_init(void) {
	// PG7 = LPUART1_TX, PG8 = LPUART1_RX, AF8 to ST-LINK VCP

	 // power avail on PG[15:2] (LPUART1)
	PWR->CR2 |= (PWR_CR2_IOSV);

	// Configure PG7 and PG8 as TX and RX
   // enable GPIOG clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);

	// alternate function
	GPIOG->MODER &= ~((3U << (2 * 7)) | (3U << (2 * 8)));
	GPIOG->MODER |=  ((2U << (2 * 7)) | (2U << (2 * 8)));

   // PG7 AF8 (TX)
	GPIOG->AFR[0] &= ~(0xFU << (4 * 7));
	GPIOG->AFR[0] |=  (8U   << (4 * 7));

	// PG8 AF8 (RX)
	GPIOG->AFR[1] &= ~(0xFU << (4 * (0)));
	GPIOG->AFR[1] |=  (8U   << (4 * (0)));

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

/* -----------------------------------------------------------------------------
 * function : LPUART_Print()
 * INs      : string message — pointer to null-terminated string to transmit
 * OUTs     : none
 * action   : Character transmission for a string message
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
void LPUART_Print( const char* message ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
         ;
      LPUART1->TDR = message[iStrIdx];       // send this character
	   iStrIdx++;                             // advance index to next char
   }
}

/* -----------------------------------------------------------------------------
 * function : LPUART_ESC_Print()
 * INs      : string command — pointer to null-terminated string to transmit
 * OUTs     : none
 * action   : Transmits ESC command
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
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

/* -----------------------------------------------------------------------------
 * function : LPUART_ESC_Print_Pos()
 * INs      : Desired row and column position
 * OUTs     : none
 * action   : moves the cursor to the desired row and column positions
 * Citation : aassisted by Claude (Anthropic), claude.ai, [5/6/2026]
 * -------------------------------------------------------------------------- */
void LPUART_ESC_Print_Pos(uint8_t row, uint8_t col) {
    char buffer[12];
    uint8_t idx = 0;

    // Manually build "\033[row;colH" without sprintf or string.h
    buffer[idx++] = '\033';
    buffer[idx++] = '[';

    // Convert row to characters
    if (row >= 10) buffer[idx++] = '0' + (row / 10);
    buffer[idx++] = '0' + (row % 10);

    buffer[idx++] = ';';

    // Convert col to characters
    if (col >= 10) buffer[idx++] = '0' + (col / 10);
    buffer[idx++] = '0' + (col % 10);

    buffer[idx++] = 'H';
    buffer[idx++] = '\0';

    LPUART_Print(buffer);
}


/* -----------------------------------------------------------------------------
 * function : LPUART1_IRQHandler()
 * INs      : none
 * OUTs     : none
 * action   : Handles key press inputs in terminal console
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
void LPUART1_IRQHandler( void  ) {
   uint8_t charRecv;
   if (LPUART1->ISR & USART_ISR_RXNE) {
      charRecv = LPUART1->RDR;
      switch ( charRecv ) {
      // Handle direction key press for character movement
	   case 'a':
			last_key = 'a';
	   	update_pos_flag = 1;
	   	break;
	   case 'w':
	   			last_key = 'w';
	   	   	update_pos_flag = 1;
	   	   	break;
	   case 's':
	   			last_key = 's';
	   	   	update_pos_flag = 1;
	   	   	break;
	   case 'd':
	   			last_key = 'd';
	   	   	update_pos_flag = 1;
	   	   	break;
	   default:
	      while( !(LPUART1->ISR & USART_ISR_TXE) )
               ;    // wait for empty TX buffer
		LPUART1->TDR = charRecv;  // echo char to terminal
	}  // end switch
   }
}