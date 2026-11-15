#ifndef MMU_H
#define MMU_H

#include <stdint.h>

/* Discriptor Bit Definitions */

/* Bits [1:0]: descriptor type */
#define DESC_VALID      (1UL << 0)                  /* bit[0] = 1: descriptor is live     */
#define DESC_TABLE      (1UL << 1)                  /* bit[1] = 1: table (next-level ptr) */
#define DESC_BLOCK      (0UL << 1)                  /* bit[1] = 0: block (terminal entry) */

/* Bit [10]: Access Flag (AF) */
#define DESC_AF         (1UL << 10)                 /* must be 1 or the MMU faults        */

/* Bits [9:8]: Shareability (SH) */
#define DESC_SH_INNER   (3UL << 8)                  /* inner shareable, normal memory     */
#define DESC_SH_NONE    (0UL << 8)                  /* non-shareable, device memory       */

/* Bits [7:6]: Access Permissions (AP) */
#define DESC_AP_RW_EL1  (0UL << 6)                  /* EL1 r/w, EL0 no access             */

/* Bits [4:2]: Memory Attribute Index (AttrIndx) */
#define DESC_ATTR(idx)  ((uint64_t)(idx) << 2)      /* 3-bit index into MAIR              */

/* Bits [54,53]: Execute-Never */
#define DESC_PXN        (1UL << 53)                 /* privileged execute never           */
#define DESC_UXN        (1UL << 54)                 /* unprivileged execute never         */

/* Composite Block Descriptors */

/* Normal write-back cacheable RAM: MAIR slot 0 */
#define BLOCK_NORMAL \
    (DESC_VALID | DESC_BLOCK | DESC_AF | DESC_SH_INNER | \
     DESC_AP_RW_EL1 | DESC_ATTR(0))

/* Device-nGnRnE MMIO: MAIR slot 1 */
#define BLOCK_DEVICE \
    (DESC_VALID | DESC_BLOCK | DESC_AF | DESC_SH_NONE | \
     DESC_AP_RW_EL1 | DESC_ATTR(1) | DESC_PXN | DESC_UXN)

/* MAIR Settings */

/* Memory Attribute Indirection Register (MAIR_EL1) */
#define MAIR_NORMAL_WB  0xFFUL                      /* Attr0: Normal write-back cacheable */
#define MAIR_DEVICE_NG  0x00UL                      /* Attr1: Device-nGnRnE */

/* Result: 0x00000000000000FF 
    Attr0 is in byte 0 and Attr1 is in byte 1 -> 0x0000000000FF with 
    MAIR_DEVICE_NG << 8 = 0x0000 -> MAIR_VALUE = 0x00FF */
#define MAIR_VALUE      ((MAIR_DEVICE_NG << 8) | MAIR_NORMAL_WB)

/* Translation Control Engine (TCR_EL1) Settings */
#define TCR_T0SZ      (16UL << 0)
#define TCR_IRGN0_WB  (1UL << 8)
#define TCR_ORGN0_WB  (1UL << 10)
#define TCR_SH0_INNER (3UL << 12)
#define TCR_TG0_4K    (0UL << 14)
#define TCR_EPD1      (1UL << 23)
#define TCR_IPS_48BIT (5UL << 32)
#define TCR_VALUE     (TCR_T0SZ | TCR_IRGN0_WB | TCR_ORGN0_WB | \
                       TCR_SH0_INNER | TCR_TG0_4K | TCR_EPD1 | TCR_IPS_48BIT)

/* Functions */
void mmu_init(void);

/*
* Builds a three-level AArch64 page table (L0-2) with an identity map
* Every virtual address equals a physical address
* 
* First GB holds the peripherals, thus maps through an L2 table of Device-nGnRnE blocks
*
* That covers teh PL011 UART at 0x09000000
* Kernel RAM at 0x40000000 maps as Normal write-back cacheable
*
* mmu_init() writes MAIR_EL1 and TCR_EL1, then sets SCTLR_EL1.M
* to bring the MMU online
*/

#endif /* MMU_H */