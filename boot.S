.section .text.boot
.global _start

_start:
    mrs     x0, CurrentEL    /* CurrentEL buts [3:2] encode the exception level */
    lsr     x0, x0, #2       /* Shift right 2 bits to isolate the EL number. EL1 = 1, EL2 = 2, EL3 = 3 */
    cmp     x0, #1           /* Compare against 1 (EL1) */
    beq     .el1_ready       /* QEMU virt drops here by default */

    mov     x0, #(1 << 31)   /* HCR_EL2.RW (bit 31) = 1 tells teh CPU that EL1 wil be 64-bit AArch64. Otherwise, runs in AArch32 */
    msr     hcr_el2, x0      /* Writes x0 into HCR_EL2. Can only write EL2 system register from EL2 or higher */

    mov     x0, #0x3c5       /* PSR_EL2 = 0x3c5: bits [9:6] mask D/A/I/F exceptions at EL1, bits [3:0] = 0b0101 select EL1h */
    msr     spsr_el2, x0

    adr     x0, .el1_ready   /* adr calculates the address of .el1_ready relative to PC */
    msr     elr_el2, x0      /* After erer, the CPU will jump to this address */

    eret

.el1_ready:
    ldr     x0, =_stack_top /* Uses the assembler literal piil to lead a 64-bit address, emits a PC-relative load */
    mov     sp, x0           /* Set the stack pointer. Allows C functions to safely push and pop */

    ldr     x1, =__bss_start /* Load the start of the BSS region. Defined in linker */
    ldr     x2, =__bss_end   /* Load end of BSS */
.zero_bss:
    cmp     x1, x2
    bge     .call_kernel     /* If x1 >= x2, reached end of BSS and jump past loop */
    str     xzr, [x1], #8    /* store 0 to *x1, then post-increment x1 by 8 */
    b       .zero_bss

.call_kernel:
    bl      kernel_main      /* b1 branches to kernel_main and saves the return address in lr (0x30). kernel_main can return here */
    b       .                /* Infinite loop that spins on the current address. If kernel_main returns, we stop here rather than executing random bytes below */
