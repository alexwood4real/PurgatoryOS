#include "drivers/uart.h"
#include "kernel/mmu.h"

void kernel_main(void) {
    mmu_init();
    uart_init();

    kprint("Hello kernel\n");

    /* Spin, nothing else to do yet */
    while (1);
}