/*
 * LPUART1.c
 *
 *  Created on: May 4, 2026
 *      Author: mickp
 */

//configures LPUART1
void LPUART1_init(void) {

PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
/* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
   select AF mode and specify which function with AFR[0] and AFR[1] */
LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
/* USER: set baud rate register (LPUART1->BRR) */
NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
__enable_irq();                          // enable global interrupts
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

