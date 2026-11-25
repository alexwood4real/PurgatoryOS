#include "drivers/uart.h"
#include "kernel/exceptions.h"
#include "kernel/heap.h"
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
    kprint("Timer: armed\n");

    /* Init Heap before any dynamic allocation */
    heap_init();
    kprint("Heap: ready\n");

    /* Smoke test: allocate, free, re-allocate */
    void *a = kmalloc(128);
    void *b = kmalloc(64);
    void *c = kmalloc(256);

    kprint("[test] allocated a, b, c\n");
    heap_dump();

    kfree(b);
    kprint("[test] freed b\n");

    void *d = kmalloc(48);
    kprint("[test] allocated d (should reuse b slot)\n");
    heap_dump();

    kfree(a); kfree(c); kfree(d);
    kprint("[test] freed all: heap should be mostly coalesced\n");
    heap_dump();

    /* Unmask IRQs at the CPU */
    asm volatile("msr daifclr, #2");
    kprint("Interrupts enabled. Waiting for timer...\n");

    /* Wait for Interrupts */
    while (1)
        {
        asm volatile("wfi");
        }
}