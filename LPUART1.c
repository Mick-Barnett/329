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
      // Handle color change request
	   case 'R':
         LPUART_ESC_Print("31m"); // Change text color to red
	      break;
	   case 'G':
         LPUART_ESC_Print("32m"); // Change text color to green
	      break;
	   case 'B':
         LPUART_ESC_Print("34m"); // Change text color to blue
	      break;
	   case 'W':
         LPUART_ESC_Print("37m"); // Change text color to white
	      break;
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

/* -----------------------------------------------------------------------------
 * function : Render_SplashScreen()
 * INs      : none
 * OUTs     : none
 * action   : Renders the splash screen
 * Citation : aassisted by Claude (Anthropic), claude.ai, [5/6/2026]
 * -------------------------------------------------------------------------- */
void Render_SplashScreen(void) {
    uint8_t i;

    // Clear screen
    LPUART_ESC_Print("2J");
    LPUART_ESC_Print("H");

    // Draw top border character by character with color cycle
    LPUART_ESC_Print_Pos(1, 1);
    for (i = 0; i < COLS; i++) {
        // Cycle through red, yellow, green, cyan, blue, magenta
        switch(i % 6) {
            case 0: LPUART_ESC_Print("31m"); break;   // red
            case 1: LPUART_ESC_Print("33m"); break;   // yellow
            case 2: LPUART_ESC_Print("32m"); break;   // green
            case 3: LPUART_ESC_Print("36m"); break;   // cyan
            case 4: LPUART_ESC_Print("34m"); break;   // blue
            case 5: LPUART_ESC_Print("35m"); break;   // magenta
        }
        LPUART_Print("*");
    }

    // Draw bottom border character by character
    LPUART_ESC_Print_Pos(ROWS, 1);
    for (i = 0; i < COLS; i++) {
        switch(i % 6) {
            case 0: LPUART_ESC_Print("31m"); break;
            case 1: LPUART_ESC_Print("33m"); break;
            case 2: LPUART_ESC_Print("32m"); break;
            case 3: LPUART_ESC_Print("36m"); break;
            case 4: LPUART_ESC_Print("34m"); break;
            case 5: LPUART_ESC_Print("35m"); break;
        }
        LPUART_Print("*");
    }

    // Draw Cascading left and right borders
    for (uint8_t j = 0; j < 9; j+=4) {
		 for (i = 2; i < ROWS; i++) {
			  switch(i % 6) {
					case 0: LPUART_ESC_Print("31m"); break;
					case 1: LPUART_ESC_Print("33m"); break;
					case 2: LPUART_ESC_Print("32m"); break;
					case 3: LPUART_ESC_Print("36m"); break;
					case 4: LPUART_ESC_Print("34m"); break;
					case 5: LPUART_ESC_Print("35m"); break;
			  }
			  LPUART_ESC_Print_Pos(i, 1 + j);
			  LPUART_Print("*");
			  LPUART_ESC_Print_Pos(i, COLS - j);
			  LPUART_Print("*");
			  delay_us(30000);   // 30ms per row
		 }
    }

    LPUART_ESC_Print("0m");   // reset color

    // Pause before title reveal
    delay_us(500000);   // 0.5 second pause

    // Title reveal — print one character at a time
    const char* title    = ">>> TERMINAL QUEST <<<";

    uint8_t idx = 0;

    LPUART_ESC_Print("1m");          // bold
    LPUART_ESC_Print("36m");         // cyan
    LPUART_ESC_Print_Pos(10, 29);    // center for 22 char title

    while (title[idx] != 0) {
        char buf[2] = {title[idx], '\0'};
        LPUART_Print(buf);
        delay_us(80000);    // 80ms per character for dramatic effect
        idx++;
    }

    LPUART_ESC_Print("0m");          // reset
}

/* -----------------------------------------------------------------------------
 * function : Render_Border()
 * INs      : none
 * OUTs     : none
 * action   : Renders the rectangular border for the game
 * -------------------------------------------------------------------------- */
void Render_Border(void) {
	uint8_t i;
	// Render top of border
	LPUART_ESC_Print_Pos(BORDER_ROW_MIN, BORDER_COL_MIN);
   for (i = 0; i < BORDER_COL_MAX - BORDER_COL_MIN + 1; i++) {
       LPUART_Print("#");
   }

   // Render bottom of border
   LPUART_ESC_Print_Pos(BORDER_ROW_MAX, BORDER_COL_MIN);
   for (i = 0; i < BORDER_COL_MAX - BORDER_COL_MIN + 1; i++) {
       LPUART_Print("#");
   }

   // Render left and right border
   for (i = 2 + BORDER_OFFSET; i < BORDER_ROW_MAX; i++) {
       LPUART_ESC_Print_Pos(i, BORDER_COL_MIN);       // left side
       LPUART_Print("#");
       LPUART_ESC_Print_Pos(i, BORDER_COL_MAX);    // right side
       LPUART_Print("#");
   }
}

/* -----------------------------------------------------------------------------
 * function : Update_Character_Position()
 * INs      : none
 * OUTs     : none
 * action   : Updates the position of the game character based on key press
 * -------------------------------------------------------------------------- */
void Update_Character_Position(void) {
	switch(last_key) {
	case 'a': char_col_pos--; break;
	case 'd': char_col_pos++; break;
	case 'w': char_row_pos--; break;
	case 's': char_row_pos++; break;
	}
	update_pos_flag = 0;
}

/* -----------------------------------------------------------------------------
 * function : Update_Character_Position()
 * INs      : none
 * OUTs     : none
 * action   : Renders the game character at the current character position
 * -------------------------------------------------------------------------- */
void Render_Character(void) {
	// Position Cursor to old character position
	 LPUART_ESC_Print_Pos(prev_char_row_pos, prev_char_col_pos);
	// Delete old character
	 LPUART_Print(" ");
	// Wrap character position if out of bounds
	 if (char_row_pos >= BORDER_ROW_MAX) {
		 char_row_pos = BORDER_ROW_MIN + 1;
	 }
	 if (char_row_pos <= BORDER_ROW_MIN) {
		 char_row_pos = BORDER_ROW_MAX - 1;
	 }
	 if (char_col_pos >= BORDER_COL_MAX) {
		 char_col_pos = BORDER_COL_MIN + 1;
	 }
	 if (char_col_pos <= BORDER_COL_MIN) {
		 char_col_pos = BORDER_COL_MAX - 1;
	 }
	// Position Cursor to new character position
	LPUART_ESC_Print_Pos(char_row_pos, char_col_pos);
	// Print character
	LPUART_Print("a");
	// Update previous character position
   prev_char_row_pos = char_row_pos;
   prev_char_col_pos = char_col_pos;

}

