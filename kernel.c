#include "uart.h"

void kernel_main(void) {
    uart_init();
    kprint("Hello kernel\n");

    /* Spin, nothing else to do yet */
    while (1);
}