#include <stdint.h>
#include "kernel/mmu.h"

/* Static Declarations */
static void clean_cache_range(uint64_t, uint64_t);

/* Page Tables*/
static uint64_t l0_table[512]       __attribute((aligned(4096)));
static uint64_t l1_table[512]       __attribute((aligned(4096)));
static uint64_t l2_dev_table[512]   __attribute((aligned(4096)));
static uint64_t l2_table[512]       __attribute((aligned(4096)));

void mmu_init()
    {
    /* Local Variables */
    int i;
    uint64_t sctlr;

    /* init tables to 0 */
    for( i = 0; i < 512; i++ )
        {
        l0_table[ i ]       = 0;
        l1_table[ i ]       = 0;
        l2_dev_table[ i ]   = 0;
        l2_table[ i ]       = 0;
        }

    /* Build L0 Table:
     * 512 entries each covering 512 GB of address space
     * all physical address range below 512 GB fits in L0[ 0 ]
     * store physical addresses of L1 combined with the valid bit and table bit
     * thus valid table descriptor
     * hardware defreferences this address to get the L1 table
     */
    l0_table[ 0 ] = (uint64_t) l1_table | DESC_VALID | DESC_TABLE;

    /* Build L1 Table:
     * L1[ 0 ] covers 0 GB to 1 GB and points at the device L2 table
     * L1[ 1 ] covers 1 GB to 2 GB and points at the kernel L2 table
     */
    l1_table[ 0 ] = (uint64_t) l2_dev_table | DESC_VALID | DESC_TABLE;
    l1_table[ 1 ] = (uint64_t) l2_dev_table | DESC_VALID | DESC_TABLE;

    /* Build L2 Peripheral Table: 
     * 512 blocks of 2 MB cover 0x00000000 to 0x3FFFFFFF
     * On QEMU virt that wole GB is memory-mapped IO
     * It holds flash, GIC, PL011 UART, at 0x90000000, RTC,
     * the virtio-mio transports and the PCIe ECAM window
     * Device aatributes stop teh CPU cachinng, mergin or reording these accesses
     * The volatile keyword in the UART drvier only constrains the compiler
     * The page tables constrain the CPU
     */
    for( i = 0; i < 512; i++ )
        {
        uint64_t pa = (uint64_t)i * 0x200000UL;
        l2_dev_table[ i ] = pa | BLOCK_DEVICE;
        }

    /* Build the Kernel L2 Table: 
     * Maps 16 MB from 0x40000000, the base of RAM on QEMU virt
     */
    for( i = 0; i < 8; i++ )
        {
        uint64_t pa = 0x40000000UL + (uint64_t)i * 0x20000000UL;
        l2_table[ i ] = pa | BLOCK_NORMAL;
        }

    /* Clean Tables */
    clean_cache_range( (uint64_t)l0_table,      4096 );
    clean_cache_range( (uint64_t)l1_table,      4096 );
    clean_cache_range( (uint64_t)l2_dev_table,  4096 );
    clean_cache_range( (uint64_t)l2_table,      4096 );

    /* Program System Registers */
    asm volatile("msr mair_el1,  %0" :: "r"(MAIR_VALUE) : "memory");
    asm volatile("msr tcr_el1,   %0" :: "r"(TCR_VALUE)  : "memory");
    asm volatile("msr ttbr0_el1, %0" :: "r"((uint64_t)l0_table) : "memory");

    /* Invalidate TLBs and Instruction Cache */
    asm volatile("tlbi vmalle1" ::: "memory");
    asm volatile("ic iallu"     ::: "memory");
    asm volatile("dsb sy"       ::: "memory");
    asm volatile("isb");

    /* Enable MMU and Caches */
    asm volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1UL << 0);  /* M - MMU */
    sctlr |= (1UL << 2);  /* C - Data Cache */
    sctlr |= (1UL << 12); /* I - Instruction Cache */
    asm volatile("msr sctlr_el1, %0" :: "r"(sctlr) : "memory");
    asm volatile("isb");
    }

/* Clean the data cache by VA to the Point of Coherency
 * The MMU table walker reads from RAM, not data cache
 * Without this it sees stale zeros and every translation faults
 */
static void clean_cache_range( uint64_t start, uint64_t size )
    {
    uint64_t line_size = 64;
    uint64_t end = start + size;
    start &= ~( line_size - 1 );

    for( uint64_t addr = start; addr < end; addr += line_size )
        {
        asm volatile( "dc cvac, %0" :: "r"(addr) : "memory" );
        }

    asm volatile( "dsb sy" ::: "memory" );
    }
