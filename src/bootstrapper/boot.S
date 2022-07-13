/*
 * Bootstrapper. For details see: https://jsandler18.github.io/explanations/boot_S.html
 * For multi core on raspi4 see: https://github.com/szediwy/raspberry-pi-os/commit/f601eb8b88a97aaaa92be90f7c01272541367fac
 */
#include "mm.h"

.section	".text.boot"

.global _start
_start:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF				// Check core id
	cbz		x0, init_bss
	wfe									// If not core 0 wait for sev instruction
	b		master

proc_hang:
	b 		proc_hang

init_bss:
	adr		x0, __bss_start 			// Init bss
	adr		x1, __bss_end
	sub		x1, x1, x0
	bl		memzero

	// Setup and wake cores
	sev

	// 	Enable the other cores see: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/arm64/booting.rst?h=v5.3#n255
	mov 	x0, #0
	adr		x0, master

	mov 	x1, #0xE0
	str		x0, [x1]
	mov		x1,	#0xE8
	str		x0, [x1]
	mov 	x1, #0xF0
	str 	x0, [x1]

master:
	mrs	x0, mpidr_el1
	and	x0, x0, #0xFF					// Get core id

	mov	x1, #SECTION_SIZE
	mul	x1, x1, x0						// Calculate SP for core
	add	x1, x1, #LOW_MEMORY
	mov	sp, x1							// Setup SP

	bl	kernel_main
	b 	proc_hang