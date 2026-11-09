#include "uart.h"

/*
* PL011's required initialization sequence:
* 1. disable first
* 2. configure baud rate and line format
* 3. re-enable
*/
void uart_init(void)
    {
    /* Disable UART before reconfiguring */
    UART_REG(UARTCR) = 0;

    /*
    * Set Baud rate. QEMU's virt machine uses a 24 MHz UART clock.
    * For 115200 baud: divisor - 24,000,000 / (16 * 115200) = 13.0208333
    *   Integer part = 13 -> UARTIBRD = 13 
    *   Fractional part = 0.0208333 * 64 = 1 -> UARTFFBRD = 1
    */
    UART_REG(UARTIBRD) = 13;
    UART_REG(UARTFBRD) = 1;

    /*
    * Line control: 8-bit word length, FIFos enabled, 1 stop bit, no parity
    * The UARTLCR_H write must happen AFTER setting baud rate, because writing
    * UARTLCR_H latches the baud rate divisor into the internal registers. 
    */
    UART_REG(UARTLCR_H) = UARTLCR_WLEN8 | UARTLCR_FEN;

    /* Re-enable: UART on, Tx on, Rx on */
    UART_REG(UARTCR) = UARTCR_UARTEN | UARTCR_TXE | UARTCR_RXE;
    }

void uart_putc(char c)
    {
    /* Spin until the transmit FIFO has space */
    while(UART_REG(UARTFR) & UARTFR_TXFF);
    UART_REG(UARTDR) = (uint32_t)c;
    }

void uart_puts(const char *s)
    {
    while(*s)
        {
        uart_putc(*s++);
        }
    }

void kprint(const char *s)
    {
    uart_puts(s);
    }