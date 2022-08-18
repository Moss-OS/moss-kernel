/*
 * Bootstrapper. For details see: https://jsandler18.github.io/explanations/boot_S.html
 * For multi core on Pi4 see: https://github.com/szediwy/raspberry-pi-os/commit/f601eb8b88a97aaaa92be90f7c01272541367fac
 */
#include "bootstrapper/mm.h"
#include "bootstrapper/sysregs.h"

.section ".text.boot"

.global _start
_start:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF					// Check core id
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

	ldr		x0, =SPSR_VALUE					// Mask interrupts and set EL1 SP
	msr		spsr_el2, x0

	adr		x0, enter_el1					// Boot will continue with init_ss at el1
	msr		elr_el2, x0

	eret

enter_el1:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF					// Get core id
	mov		x1, #SECTION_SIZE
	mul		x1, x1, x0						// Calculate SP for core
	add		x1, x1, #LOW_MEMORY
	mov		sp, x1							// Setup SP

	bl		kernel_main
	b 		proc_hang