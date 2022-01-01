/*
 * Bootstrapper. For details see: https://jsandler18.github.io/explanations/boot_S.html
 */
#include "mm.h"
#include "smp.h"

.section	".text.boot"

.global _start
_start:
	mrs		x0, mpidr_el1
	and		x0, x0, #0xFF				// Check core id
	mov		x19, x0						// Save core id
	cbz		x0, start_primary_core
	b		start_secondary_cores

start_secondary_cores:
	mrs 	x19, mpidr_el1
	and		x19, x19, #0xFF				// Check core id
	mov		x0, STACK_SIZE
	mul		x2, x0, x19					// Calculate SP for each core
	mov		sp, x2						// Setup SP
	mov		x1, #30000
	mul		x0, x1, x19					// Calc delay for each core
	bl		delay
	mov		x0, x19
	bl		kernel_secondary			// To the kernel secondary core entry

proc_hang:
	b 		proc_hang

start_primary_core:
	adr		x0, __bss_start 			// Init bss
	adr		x1, __bss_end
	sub		x1, x1, x0
	bl		memzero

	// Setup and wake cores
	mov		sp, #LOW_MEMORY
	bl		kernel_main					// To the kernel primary core entry
	mov 	x0, #2						// Tell core 2 ...
	adr		x1, start_secondary_cores	// ... where to start
	bl		wakeup_core
	mov		x0, #3
	bl 		wakeup_core
	b		proc_hang					// should never come here