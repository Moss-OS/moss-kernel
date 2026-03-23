/*
 * Bootstrapper. For details see: https://jsandler18.github.io/explanations/boot_S.html
 * For multi core on Pi4 see: https://github.com/szediwy/raspberry-pi-os/commit/f601eb8b88a97aaaa92be90f7c01272541367fac
 */
#include "arm/mmu.h"
#include "bootstrapper/mm.h"
#include "bootstrapper/sysregs.h"
#include "peripherals/base.h"

.section ".text.boot"

.globl _start
_start:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF					// Check core id
	cbz		x0, master
	b		proc_hang						// Skip enabling other cores for now
	cbz		x0, init_bss
	wfe										// If not core 0 wait for sev instruction
	b		master

proc_hang:
	b 		proc_hang

init_bss: 									// currently unused, was needed for using multiple cores
	adrp	x0, __bss_start 				// Init bss
	add		x0, x0, :lo12:__bss_start
	adrp	x1, __bss_end
	add		x1, x1, :lo12:__bss_end
	sub		x1, x1, x0
	bl		memzero

	sev 									// 	Enable the other cores see: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/arm64/booting.rst?h=v5.3#n255

	mov 	x0, #0
	adr		x0, master

	mov 	x1, #0xE0
	str		x0, [x1]
	mov		x1,	#0xE8
	str		x0, [x1]
	mov 	x1, #0xF0
	str 	x0, [x1]

master:  									// Setup EL https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson02/rpi-os.md
	ldr		x0, =SCTLR_VALUE_MMU_DISABLED	// Config el1 little endian, diable instruction and data cache, disable MMU
	msr		sctlr_el1, x0

	mrs 	x0, CurrentEL
    lsr 	x0, x0, #2
	cmp 	x0, #3
	beq 	el3

	mrs		x0, hcr_el2
	orr		x0, x0, #(1<<31)
	msr		hcr_el2, x0

	mov 	x0, #SPSR_VALUE
	msr		spsr_el2, x0

	adr		x0, enter_el1
	msr		elr_el2, x0
	eret

el3:
	ldr		x0, =SCR_VALUE
	msr		scr_el3, x0

	ldr		x0, =SPSR_VALUE					// Mask interrupts and set EL1 SP
	msr		spsr_el3, x0

	adr		x0, enter_el1					// Boot will continue with init_ss at el1
	msr		elr_el3, x0

	eret

enter_el1:
	adrp	x0, __bss_start 				// Init bss
	add		x0, x0, :lo12:__bss_start
	adrp	x1, __bss_end
	add		x1, x1, :lo12:__bss_end
	sub		x1, x1, x0
	bl		memzero

	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF					// Get core id
	mov		x1, #SECTION_SIZE
	mul		x1, x1, x0						// Calculate SP for core
	add		x1, x1, #LOW_MEMORY
	mov		sp, x1							// Setup SP

	bl		__create_idmap
	adrp	x0, idmap_dir
	msr		ttbr0_el1, x0

	bl 		__create_page_tables

	mov		x0, #VA_START
	add		sp, x0, #LOW_MEMORY

	adrp	x0, pg_dir
	msr		ttbr1_el1, x0

	ldr		x0, =(TCR_VALUE)
	msr		tcr_el1, x0

	ldr		x0, =(MAIR_VALUE)
	msr		mair_el1, x0


	ldr		x2, =kernel_main

	mrs		x0, mpidr_el1
	and		x19, x0, #0xFF						// Save core id in callee-saved register

	tlbi	vmalle1is								// Invalidate all TLB entries
	dsb		ish										// Ensure TLB invalidation completed
	isb												// Synchronize context

	ldr		x0, =(SCTLR_VALUE_MMU_ENABLED)
	msr		sctlr_el1, x0

	isb												// Ensure MMU enabled before next instruction

	mov		x0, x19									// Pass core id as first argument to kernel_main
	br 		x2

.macro create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
.endm

.macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr		\tmp1, \virt, #\shift
	and		\tmp1, \tmp1, #PTRS_PER_TABLE - 1				// table index
	add		\tmp2, \tbl, #PAGE_SIZE
	orr		\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE
	str		\tmp2, [\tbl, \tmp1, lsl #3]
	add		\tbl, \tbl, #PAGE_SIZE							// next level table page
.endm

.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	lsr		\start, \start, #SECTION_SHIFT
	and		\start, \start, #PTRS_PER_TABLE - 1				// table index
	lsr		\end, \end, #SECTION_SHIFT
	and		\end, \end, #PTRS_PER_TABLE - 1					// table end index
	lsr		\phys, \phys, #SECTION_SHIFT
	mov		\tmp1, #\flags
	orr		\phys, \tmp1, \phys, lsl #SECTION_SHIFT			// table entry
9999:
	str		\phys, [\tbl, \start, lsl #3]					// store the entry
	add		\start, \start, #1								// next entry
	add		\phys, \phys, #SECTION_SIZE						// next block
	cmp		\start, \end
	b.ls	9999b
.endm

__create_idmap:												// Identiy paging (https://wiki.osdev.org/Identity_Paging) for qemu
	mov		x29, x30

	adrp	x0, idmap_dir
	mov		x1, #PG_DIR_SIZE
	bl		memzero

	adrp	x9, idmap_dir										// L0 table
	add		x10, x9, #PAGE_SIZE							// L1 (PUD)
	orr		x11, x10, #MM_TYPE_PAGE_TABLE
	str		x11, [x9]

	add		x12, x9, #(2 * PAGE_SIZE)					// L2 PMD for 0x00000000-0x3FFFFFFF
	orr		x13, x12, #MM_TYPE_PAGE_TABLE
	str		x13, [x10]										// PUD[0]

	add		x14, x9, #(3 * PAGE_SIZE)					// L2 PMD for 0xC0000000-0xFFFFFFFF
	orr		x15, x14, #MM_TYPE_PAGE_TABLE
	mov		x16, #3
	str		x15, [x10, x16, lsl #3]					// PUD[3]

	/* Map RAM and kernel image as normal memory */
	mov		x1, xzr
	mov		x2, xzr
	ldr		x3, =(PI3_DEVICE_BASE - SECTION_SIZE)
	create_block_map x12, x1, x2, x3, MMU_FLAGS, x4

	/* Map Pi2/Pi3 device memory window with device attributes */
	ldr		x1, =PI3_DEVICE_BASE
	ldr		x2, =PI3_DEVICE_BASE
	ldr		x3, =(0x40000000 - SECTION_SIZE)
	create_block_map x12, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	/* Map Pi4 high device/GIC window with device attributes */
	ldr		x1, =PI4_DEVICE_BASE
	ldr		x2, =PI4_DEVICE_BASE
	ldr		x3, =(PHYS_MEMORY_SIZE - SECTION_SIZE)
	create_block_map x14, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov		x30, x29
	ret

__create_page_tables:
	mov		x29, x30										// save return address

	adrp	x0, pg_dir
	mov		x1, #PG_DIR_SIZE
	bl 		memzero

	adrp	x9, pg_dir
	ldr		x1, =VA_START
	create_table_entry x9, x1, PGD_SHIFT, x2, x3				// L0->L1 for kernel VA range

	add		x10, x9, #PAGE_SIZE							// L2 PMD for VA_START+0x0-0x3FFFFFFF
	orr		x11, x10, #MM_TYPE_PAGE_TABLE
	str		x11, [x9]										// PUD[0]

	add		x12, x9, #(2 * PAGE_SIZE)					// L2 PMD for VA_START+0xC0000000-0xFFFFFFFF
	orr		x13, x12, #MM_TYPE_PAGE_TABLE
	mov		x14, #3
	str		x13, [x9, x14, lsl #3]						// PUD[3]

	/* Map kernel image and low RAM as normal memory */
	mov 	x1, xzr
	ldr 	x2, =VA_START
	ldr		x3, =(VA_START + PI3_DEVICE_BASE - SECTION_SIZE)
	create_block_map x10, x1, x2, x3, MMU_FLAGS, x4

	/* Map Pi2/Pi3 device window as device memory */
	ldr 	x1, =PI3_DEVICE_BASE
	ldr 	x2, =(VA_START + PI3_DEVICE_BASE)
	ldr		x3, =(VA_START + 0x40000000 - SECTION_SIZE)
	create_block_map x10, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	/* Map Pi4 high device/GIC window as device memory */
	ldr 	x1, =PI4_DEVICE_BASE
	ldr 	x2, =(VA_START + PI4_DEVICE_BASE)
	ldr		x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)
	create_block_map x12, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov	x30, x29												// restore return address
	ret