#include <stdint.h>
#include "drivers/uart.h"
#include "kernel/exceptions.h"
#include "kernel/gic.h"
#include "kernel/timer.h"

/* Defined in vectors.S */
extern void vectors(void);

/* Statics */
static void print_hex(uint64_t val )
    {
    const char hex[] = "0123456789abcdef";
    kprint("0x");
    for( int i = 60; i >= 0; i -= 4 )
        {
        uart_putc( hex[ (val >> i ) & 0xF ] );
        }
    }

void exceptions_init(void)
    {
    /**
     * Load the address of or vector table into VBAR_EL1
     * Any exception taken after this instruction uses the table
     * The isb() ensures no subsequent instructions execute before
     * this register write is visible to the CPU's fetch pipeline
     */

    asm volatile
        (
        "msr vbar_el1, %0\n"
        "isb\n"
        :: "r"((uint64_t)vectors)
        : "memory"
        );
    }

void irq_handler(void)
    {
    /* Acknowledge interrupt */
    uint32_t iar = gic_acknowledge();
    uint32_t irq = iar & 0x3FF; /* INTID is bits [9:0] */

    if( irq == TIMER_IRQ )
        {
        timer_tick();
        }
    else 
        {
        /* unknown interrupt */
        kprint("[irq] unhandled INTD: ");
        }

    /* Signal end-of-interrupt */
    gic_end(iar);
    }

void sync_exception_handler(void)
    {
    uint64_t esr, elr, far;

    asm volatile("mrs %0, esr_el1" : "=r"(esr));
    asm volatile("mrs %0, elr_el1" : "=r"(elr));
    asm volatile("mrs %0, far_el1" : "=r"(far));

    uint32_t ec = (esr >> 26) & 0x3F;
    uint32_t iss = esr & 0x01FFFFFF;

    kprint("\n[SYNC EXCEPTION]\n");
    kprint("ESR_EL1 = "); print_hex(esr); kprint("\n");
    kprint("ELR_EL1 = "); print_hex(elr); kprint("\n");
    kprint("FAR_EL1 = "); print_hex(far); kprint("\n");
    kprint("EC =  "); print_hex(ec); kprint("\n");
    kprint("ISS = "); print_hex(iss); kprint("\n");

    /* Spin: can't recover from an unhandled fault */
    while(1);
    }