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

    /* TEST: Initialize the heap and allocate three blocks */
    heap_init();

    void *a = kmalloc(128);
    void *b = kmalloc(64);
    void *c = kmalloc(512);

    kprint("a = "); kprint_ptr(a);
    kprint("b = "); kprint_ptr(b);
    kprint("c = "); kprint_ptr(c);
    kprint("\n");

    /* Free b: no-op bump allocator */
    kfree(b);

    /* Reallocate: will not resuse b's last slot */
    void *d = kmalloc(64);
    kprint("d = "); kprint_ptr(d);
    kprint(" (b slot not reused)\n");

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