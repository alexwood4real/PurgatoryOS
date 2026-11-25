#ifndef KERNEL_HEAP_H
#define KERNEL_HEAP_H

#include <stddef.h>
#include <stdint.h>

/* Align a size up to the nearest multiple of 'align' */
#define ALIGN_UP(n, align) (((size_t)(n) + (align) - 1) & ~((size_t)(align) - 1))

/* Minimum heap alignment */
#define HEAP_ALIGN 8

/* Must be called once from main before any kmalloc */
void heap_init(void);

/* Allocate 'size' bytes from kernel heap */
void *kmalloc(size_t size);

/* release memory previously returned by kmalloc */
void kfree(void *ptr);

/* print allocator state to UART for debugging */
void heap_dump(void);

#endif