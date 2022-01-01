.globl get_core_id
get_core_id:
	mrs x0, mpidr_el1
	and x0, x0, #0xFF
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret