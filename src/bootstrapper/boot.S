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

init_bss:
	adr		x0, __bss_start 				// Init bss
	adr		x1, __bss_end
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

	ldr		x0, =HCR_VALUE					// Config el2 (hypervisor) to AArch64
	msr		hcr_el2, x0

	ldr		x0, =SCR_VALUE
	msr		scr_el3, x0

	ldr		x0, =SPSR_VALUE					// Mask interrupts and set EL1 SP
	msr		spsr_el3, x0

	adr		x0, enter_el1					// Boot will continue with init_ss at el1
	msr		elr_el3, x0

	eret

enter_el1:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF					// Get core id
	mov		x1, #SECTION_SIZE
	mul		x1, x1, x0						// Calculate SP for core
	add		x1, x1, #LOW_MEMORY
	mov		sp, x1							// Setup SP

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

	mov		x0, #SCTLR_MMU_ENABLED
	msr		sctlr_el1, x0

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

__create_page_tables:
	mov		x29, x30										// save return address

	adrp	x0, pg_dir
	mov		x1, #PG_DIR_SIZE
	bl 		memzero

	adrp	x0, pg_dir
	mov		x1, #VA_START
	create_pgd_entry x0, x1, x2, x3

	/* Mapping kernel and init stack*/
	mov 	x1, xzr												// start mapping from physical offset 0
	mov 	x2, #VA_START										// first virtual address
	ldr		x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE									// start mapping from device base address
	ldr 	x2, =(VA_START + DEVICE_BASE)						// first virtual address
	ldr		x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov	x30, x29												// restore return address
	ret