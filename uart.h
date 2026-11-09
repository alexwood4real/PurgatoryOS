#ifndef UART_H
#define UART_H

#include <stdint.h>

/* PL011 UART base address on QEMU virt */
#define UART_BASE 0x09000000UL

/* Register offsets */
#define UARTDR      0x000  /* Data Register */
#define UARTFR      0x018  /* Flag Register */
#define UARTIBRD    0x024  /* Integer Baud Rate Divisor */
#define UARTFBRD    0x028  /* Fractional Baud Rate Divisor */
#define UARTLCR_H   0x02C  /* Line Control Register */
#define UARTCR      0x030  /* Control Register */

/* UARTFR bits */
#define UARTFR_TXFF  (1 << 5)  /* Transmit FIFO Full */
#define UARTFR_BUSY  (1 << 3)  /* UART busy */

/* UARTLCR_H bits */
#define UARTLCR_FEN   (1 << 4)  /* Enable FIFOs */
#define UARTLCR_WLEN8 (3 << 5)  /* Word length: 8 bits */

/* UARTCR bits */
#define UARTCR_UARTEN (1 << 0)  /* UART Enable */
#define UARTCR_TXE    (1 << 8)  /* Transmit Enable */
#define UARTCR_RXE    (1 << 9)  /* Receive Enable */

/* read a PL011 register */
#define UART_REG(offset) \
    (*(volatile uint32_t *)(UART_BASE + (offset)))

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void kprint(const char *s);

#endif /* UART_H */