#include "kernel/heap.h"
#include "drivers/uart.h"
#include <stdint.h>

/* Linker-script symbols: address of heap */
extern char __heap_start[];
extern char __heap_end[];

/* Internal State: next free byte */
static char *bump_ptr;

void heap_init(void)
    {
    bump_ptr = __heap_start;

    kprint("[heap] init: bump allocator\n");
    kprint("[heap] start: 0x");
    kprint("\n");
    }

void *kmalloc(size_t size)
    {
    if( size == 0 )
        {
        return NULL;
        }

    /* Bounds check */
    if( bump_ptr + size > __heap_end )
        {
        kprint("[heap] OOM: kmalloc returning NULL\n");
        return NULL;
        }

    void *ptr = bump_ptr;
    bump_ptr += size;

    return ptr;
    }

void kfree(void *ptr)
    {
    /* Bump allocator cannot free individual blocks */
    (void)ptr;
    }

void heap_dump(void)
    {
    kprint("[heap] bump allocator: free bump_ptr to __heap_end\n");
    }

