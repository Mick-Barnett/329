/*
 *******************************************************************************
 * EE 329 A9 I2C & EEPROM
 *******************************************************************************
 * @file           : UART.c
 * @brief          : Src file for UART code
 * project         : EE 329 S'26 A9
 * authors         : Facundo Soto-Wang
 * version         : 0.3
 * date            : 05/18/2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * PIN ASSIGNMENTS
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#include "UART.h"

uint8_t USER_FLAG = 0; //user flag to transition between screens


void UART1_INIT(void){
	PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
	/* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
	   select AF mode and specify which function with AFR[0] and AFR[1] */
	UART_PORT -> MODER   &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
	UART_PORT -> MODER   |=  (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);

	UART_PORT -> OTYPER  &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);

	UART_PORT -> PUPDR   &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
	UART_PORT -> PUPDR   |= (GPIO_PUPDR_PUPD7_0| GPIO_PUPDR_PUPD8_0);

	UART_PORT -> OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos) |
									(3 << GPIO_OSPEEDR_OSPEED8_Pos));
	UART_PORT ->AFR[0] &= ~(0x000F << GPIO_AFRL_AFSEL7_Pos);
	UART_PORT ->AFR[0] |=  (0x0008 << GPIO_AFRL_AFSEL7_Pos);
	UART_PORT ->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos);
	UART_PORT ->AFR[1] |=  (0x0008 << GPIO_AFRH_AFSEL8_Pos);

	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
	LPUART1-> BRR = (BAUD_DIV); /* USER: set baud rate register (LPUART1->BRR) */
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                          // enable global interrupts
}


void LPUART_Print( const char* message ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
         ;
      LPUART1->TDR = message[iStrIdx];       // send this character
	iStrIdx++;                             // advance index to next char
   }
}

void UART_Print_Char(char c)
{
    while (!(LPUART1->ISR & USART_ISR_TXE));
    LPUART1->TDR = c;
}

//moves cursor desired distance "forward", "back", "down" or "up" n times
//then prints desired test at cursor position
void LPUART_MOVE_CURSOR(int command, char* n){
	UART_Print_Char('\x1b');
	UART_Print_Char('[');
	LPUART_Print(n);
	switch (command){
		case up:
			LPUART_Print("A");
			break;

		case down:
			LPUART_Print("B");
			break;

		case forward :
			LPUART_Print("C");
		break;

		case back :
			LPUART_Print("D");
			break;
	}

}

void LPUART_SPLASH(void){
	LPUART_Print(RESET); //clear screen and return cursor to home
	LPUART_Print(CLEAR_ATT);
	LPUART_MOVE_CURSOR(down,"18");
	LPUART_MOVE_CURSOR(forward, "86");
	delay_us(250000);
	LPUART_Print5();
	LPUART_Print(HOME);
	LPUART_MOVE_CURSOR(down,"22");
	LPUART_MOVE_CURSOR(forward, "93");
	LPUART_Print5();
	delay_us(250000);
	LPUART_Print(HOME);
	LPUART_MOVE_CURSOR(down,"26");
	LPUART_MOVE_CURSOR(forward, "100");
	LPUART_Print5();
	delay_us(250000);
	LPUART_Print(HOME);
	LPUART_MOVE_CURSOR(down,"28");
	LPUART_MOVE_CURSOR(forward, "84");
	LPUART_Print(BOLD);
	delay_us(250000);
	LPUART_Print("BURGERS ");
	LPUART_Print("BEERS ");
	LPUART_Print("MILES");
	LPUART_Print(BLINKING);
	LPUART_Print(HOME);
	LPUART_MOVE_CURSOR(down,"30");
	LPUART_MOVE_CURSOR(forward, "90");
	LPUART_Print("LOADING");
	LPUART_Print(HOME);
	LPUART_MOVE_CURSOR(down,"32");
	LPUART_MOVE_CURSOR(forward, "78");
	LPUART_Print(CLEAR_ATT);
	it_count=10;
	while(it_count>0){
		delay_us(300000);
		LPUART_Print("---");
		it_count--;
	}
}

void BOUNDS(void){
	LPUART_Print(RESET);
	LPUART_MOVE_CURSOR(forward,"1");
	LPUART_MOVE_CURSOR(down,"1");
	it_count=186;
	while(it_count>0){
			LPUART_Print("-");
			it_count--;
	}
	it_count=47;
	while(it_count>0){
		LPUART_Print("|");
		LPUART_MOVE_CURSOR(down,"1");
		LPUART_MOVE_CURSOR(back,"1");
		it_count--;
	}
	LPUART_Print(HOME);
	it_count=47;
	LPUART_MOVE_CURSOR(down,"1");
	while(it_count>0){
		LPUART_Print("|");
		LPUART_MOVE_CURSOR(down,"1");
		LPUART_MOVE_CURSOR(back,"1");
		it_count--;
		}
	it_count=186;
	LPUART_MOVE_CURSOR(up,"1");
	LPUART_MOVE_CURSOR(forward,"1");
	while(it_count>0){
		LPUART_Print("-");
		it_count--;
	}
}


void DEL_CHAR(void){ //deletes character that cursor is on top of
	PRINT_IN_PLACE(" ");
}

void PRINT_IN_PLACE(const char* a){
	LPUART_Print(a);
	LPUART_MOVE_CURSOR(back,"1");
}


void GOTO_XY(uint8_t x, uint8_t y){
    UART_Print_Char('\x1b');
    UART_Print_Char('[');

    char buffer[20];
    sprintf(buffer, "%d;%dH", y, x);

    LPUART_Print(buffer);
}

void LPUART1_IRQHandler(void)
{
    uint8_t charRecv;

    if (LPUART1->ISR & USART_ISR_RXNE)
    {
        charRecv = LPUART1->RDR;

        switch (charRecv)
        {
        case 'R':
            LPUART_Print(RED);
            break;

        case 'G':
            LPUART_Print(GREEN);
            break;

        case 'B':
            LPUART_Print(BLUE);
            break;

        case 'W':
            LPUART_Print(WHITE);
            break;

        // ✔ START GAME FLAG (ONLY HERE)
        case '1':
            USER_FLAG = 1;
            break;

        // ================= MOVEMENT =================

        case 'w':
        case 'a':
        case 's':
        case 'd':

            GOTO_XY(Cursor_x, Cursor_y);
            DEL_MAN();

            if (charRecv == 'w')
                Cursor_y = (Cursor_y == 4) ? 46 : Cursor_y - 1;

            else if (charRecv == 's')
                Cursor_y = (Cursor_y == 46) ? 4 : Cursor_y + 1;

            else if (charRecv == 'a')
                Cursor_x = (Cursor_x == 3) ? 186 : Cursor_x - 1;

            else if (charRecv == 'd')
                Cursor_x = (Cursor_x == 186) ? 3 : Cursor_x + 1;

            GOTO_XY(Cursor_x, Cursor_y);
            Print_MAN();

            // ================= RED TIMER =================
            if (red_timer > 0)
            {
                red_timer--;

                if (red_timer == 0)
                {
                    LPUART_Print(WHITE);
                }
            }
            break;

        default:
            while (!(LPUART1->ISR & USART_ISR_TXE));
            LPUART1->TDR = charRecv;
            break;
        }
    }
}

