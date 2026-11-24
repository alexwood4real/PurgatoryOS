#include "drivers/uart.h"
#include "kernel/exceptions.h"
#include "kernel/mmu.h"
#include "kernel/gic.h"
#include "kernel/timer.h"

void kernel_main(void) {
    /* Pages must be in place for any faults to get handled properly */
    mmu_init();

    /* Need the UART up for any debugging */
    uart_init();
    kprint("PurgatoryOS booting...\n");

    /* Install VBAR_EL1 before we unmask interrupts */
    exceptions_init();
    kprint("Exceptions: ready\n");

    /* GIC: enable the distributor and CPU interfaces */
    gic_init();
    kprint("GIC: ready\n");

    /* Timer: 500 ms interval, fires INTID 30 */
    timer_init(500u);
    kprint("Timer: armred\n");

    /* Unmask IRQs at the CPU */
    asm volatile("msr daifclr, #2");

    /* Wait for Interrupts */
    while (1)
        {
        asm volatile("wfi");
        }
}