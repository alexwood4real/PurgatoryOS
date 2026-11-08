.section .text.boot
.global _start

_start:
    /* Point to the stack pointer at the top of the reserved stack */
    ldr x0, =_stack_top
    mov sp, x0

    /*
    * QEMU semihosting: SYS_EXIT call.
    *
    * Semihosting lets bare-metal code make requests of the host
    * (QEMU, in this case) through a special trap instruction.
    * We use it here to ask QEMU to exit cleanly with code 0.
    * This is only useful for testing, real kernels don't exit.
    *
    * The parameter block lives on the stack:
    *   [sp+0]  reason:         0x0002026   (ADP_Stopped_ApplicationExit)
    *   [sp+8]  exist_status:   0x00000000  (success)
    */
    mov x1, #0x26
    movk x1, #0x2, lsl #16      /* x1 = 0x0002026 */
    str x1, [sp, #-16]!         /* push reason; pre-decrement sp by 16 */
    mov x0, #0
    str x0, [sp, #8]            /* push exit status */

    mov x1, sp                  /* x1 -> start of parameter block */
    mov w0, #0x18               /* wo = SYS_EXIT */
    hlt #0xf000                 /* semihosting trap, A64 encoding */