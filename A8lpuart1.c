
#include "LPUART1.h"

// game-state globals carried over from the A7 base project
extern volatile bool update_pos_flag;
volatile uint8_t last_key = 0;
uint8_t char_row_pos      = ROWS / 2;
uint8_t char_col_pos      = COLS / 2;
uint8_t prev_char_row_pos = ROWS / 2;
uint8_t prev_char_col_pos = COLS / 2;

/* -----------------------------------------------------------------------------
 * function : LPUART1_init()
 * action   : bring up LPUART1 on PG7/PG8 (AF8) to the ST-LINK VCP
 * Citation : adapted from [Lab Manual] pg 30
 * -------------------------------------------------------------------------- */
void LPUART1_init(void) {
    // enable I/O power on PG[15:2]
    PWR->CR2 |= (PWR_CR2_IOSV);

    // PG7 (TX) / PG8 (RX) to alternate function AF8
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);
    GPIOG->MODER &= ~((3U << (2 * 7)) | (3U << (2 * 8)));
    GPIOG->MODER |=  ((2U << (2 * 7)) | (2U << (2 * 8)));
    GPIOG->AFR[0] &= ~(0xFU << (4 * 7));
    GPIOG->AFR[0] |=  (8U   << (4 * 7));
    GPIOG->AFR[1] &= ~(0xFU << (4 * 0));
    GPIOG->AFR[1] |=  (8U   << (4 * 0));
    GPIOG->OTYPER  &= ~((1U << 7) | (1U << 8));
    GPIOG->PUPDR   &= ~((3U << (2 * 7)) | (3U << (2 * 8)));
    GPIOG->OSPEEDR |=  ((3U << (2 * 7)) | (3U << (2 * 8)));

    // enable clock, 8-bit data, TX/RX, RX interrupt
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
    LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
    LPUART1->CR1 |= USART_CR1_UE;
    LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    LPUART1->CR1 |= USART_CR1_RXNEIE;
    LPUART1->ISR &= ~(USART_ISR_RXNE);

    // baud rate: 115200 @ 24 MHz LPUARTCLK
    LPUART1->BRR = 0xD055;

    // enable the LPUART1 interrupt
    NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));
    __enable_irq();
}

/* -----------------------------------------------------------------------------
 * function : LPUART_Print()
 * action   : transmit a null-terminated string, one char at a time
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
void LPUART_Print( const char* message ) {
    uint16_t iStrIdx = 0;
    while ( message[iStrIdx] != 0 ) {
        while (!(LPUART1->ISR & USART_ISR_TXE))
            ;
        LPUART1->TDR = message[iStrIdx];
        iStrIdx++;
    }
}

/* -----------------------------------------------------------------------------
 * function : LPUART_ESC_Print()
 * action   : transmit an ESC [ command sequence
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
void LPUART_ESC_Print( const char* command ) {
    uint16_t iStrIdx = 0;

    // send the ESC [ prefix
    while (!(LPUART1->ISR & USART_ISR_TXE))
        ;
    LPUART1->TDR = 0x1B;
    while (!(LPUART1->ISR & USART_ISR_TXE))
        ;
    LPUART1->TDR = '[';

    // send the command body
    while ( command[iStrIdx] != 0 ) {
        while (!(LPUART1->ISR & USART_ISR_TXE))
            ;
        LPUART1->TDR = command[iStrIdx];
        iStrIdx++;
    }
}

/* -----------------------------------------------------------------------------
 * function : LPUART_ESC_Print_Pos()
 * action   : move the terminal cursor to (row, col)
 * Citation : assisted by Claude (Anthropic), claude.ai, [5/6/2026]
 * -------------------------------------------------------------------------- */
void LPUART_ESC_Print_Pos(uint8_t row, uint8_t col) {
    char buffer[12];
    uint8_t idx = 0;

    // build "\033[row;colH" manually (no sprintf / string.h)
    buffer[idx++] = '\033';
    buffer[idx++] = '[';
    if (row >= 10) buffer[idx++] = '0' + (row / 10);
    buffer[idx++] = '0' + (row % 10);
    buffer[idx++] = ';';
    if (col >= 10) buffer[idx++] = '0' + (col / 10);
    buffer[idx++] = '0' + (col % 10);
    buffer[idx++] = 'H';
    buffer[idx++] = '\0';

    LPUART_Print(buffer);
}

/* -----------------------------------------------------------------------------
 * function : ADC_putRow()             [A8 step 3]
 * action   : print one table row "LBL  CCCC  D.DDD V" (no sprintf / itoa)
 * Citation : assisted by Claude (Anthropic), claude.ai, [5/28/2026]
 * -------------------------------------------------------------------------- */
static void ADC_putRow( const char* label, uint16_t count, uint16_t mV ) {
    char buf[24];
    uint8_t i = 0;

    // 3-char row label
    buf[i++] = label[0];
    buf[i++] = label[1];
    buf[i++] = label[2];
    buf[i++] = ' ';
    buf[i++] = ' ';

    // 4-digit zero-padded ADC count
    buf[i++] = '0' + (count / 1000) % 10;
    buf[i++] = '0' + (count / 100 ) % 10;
    buf[i++] = '0' + (count / 10  ) % 10;
    buf[i++] = '0' + (count       ) % 10;
    buf[i++] = ' ';
    buf[i++] = ' ';

    // voltage as X.XXX V
    buf[i++] = '0' + (mV / 1000);
    buf[i++] = '.';
    buf[i++] = '0' + (mV / 100) % 10;
    buf[i++] = '0' + (mV / 10 ) % 10;
    buf[i++] = '0' + (mV      ) % 10;
    buf[i++] = ' ';
    buf[i++] = 'V';

    // trailing pad (wipes stale chars) + CRLF
    buf[i++] = ' ';
    buf[i++] = ' ';
    buf[i++] = '\r';
    buf[i++] = '\n';
    buf[i]   = '\0';

    LPUART_Print( buf );
}

/* -----------------------------------------------------------------------------
 * function : ADC_putCoil()            [A8 step 10b]
 * action   : print "coil current = X.XXX A" to 0.001 A (no sprintf / itoa)
 * Citation : assisted by Claude (Anthropic), claude.ai, [5/28/2026]
 * -------------------------------------------------------------------------- */
static void ADC_putCoil( uint16_t mA ) {
    char buf[16];
    uint8_t i = 0;

    // current as X.XXX A
    buf[i++] = '0' + (mA / 1000);
    buf[i++] = '.';
    buf[i++] = '0' + (mA / 100) % 10;
    buf[i++] = '0' + (mA / 10 ) % 10;
    buf[i++] = '0' + (mA      ) % 10;
    buf[i++] = ' ';
    buf[i++] = 'A';

    // trailing pad + CRLF
    buf[i++] = ' ';
    buf[i++] = ' ';
    buf[i++] = '\r';
    buf[i++] = '\n';
    buf[i]   = '\0';

    LPUART_Print( "coil current = " );
    LPUART_Print( buf );
}

/* -----------------------------------------------------------------------------
 * function : ADC_Render()             [A8 steps 3 + 10b]
 * action   : redraw the live 3-column table plus coil current; call once per
 *            processed batch. Send ESC "2J" once at startup to clear screen.
 * Citation : assisted by Claude (Anthropic), claude.ai, [5/28/2026]
 * -------------------------------------------------------------------------- */
void ADC_Render( uint16_t min_cnt, uint16_t max_cnt, uint16_t avg_cnt,
                 uint16_t min_mV,  uint16_t max_mV,  uint16_t avg_mV,
                 uint16_t coil_mA ) {
    // park cursor so the table overwrites in place each refresh
    LPUART_ESC_Print_Pos( 2, 1 );
    LPUART_Print( "ADC counts volts\r\n" );

    // three data rows then the coil-current line
    ADC_putRow( "MIN", min_cnt, min_mV );
    ADC_putRow( "MAX", max_cnt, max_mV );
    ADC_putRow( "AVG", avg_cnt, avg_mV );
    ADC_putCoil( coil_mA );
}

/* -----------------------------------------------------------------------------
 * function : LPUART1_IRQHandler()
 * action   : on RX, movement keys set update flag; other keys echo back
 * Citation : adapted from [Lab Manual] pg 31
 * -------------------------------------------------------------------------- */
void LPUART1_IRQHandler( void ) {
    uint8_t charRecv;
    if (LPUART1->ISR & USART_ISR_RXNE) {
        charRecv = LPUART1->RDR;

        // movement keys flag main to update character position
        switch ( charRecv ) {
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

        // any other key is echoed to the terminal
        default:
            while ( !(LPUART1->ISR & USART_ISR_TXE) )
                ;
            LPUART1->TDR = charRecv;
        }
    }
}
