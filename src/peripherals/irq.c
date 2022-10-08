#include "common/utils.h"
#include "common/pi.h"
#include "common/printf.h"
#include "peripherals/timer.h"
#include "kernel/entry.h"
#include "peripherals/irq.h"
#include <stdint.h>

const char entry_error_messages[18][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",
	"FIQ_INVALID_EL1t",
	"ERROR_INVALID_EL1T",

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",
	"FIQ_INVALID_EL1h",
	"ERROR_INVALID_EL1h",

	"SYNC_INVALID_EL0_64",
	"IRQ_INVALID_EL0_64",
	"FIQ_INVALID_EL0_64",
	"ERROR_INVALID_EL0_64",

	"SYNC_INVALID_EL0_32",
	"IRQ_INVALID_EL0_32",
	"FIQ_INVALID_EL0_32",
	"ERROR_INVALID_EL0_32",

	"SYNC_ERROR",
	"SYSCALL_ERROR"
};

void enable_interrupt(unsigned int irq) {
	printf("Interrupt %x enabled\r\n", irq);
	unsigned int n = irq / 32;
	unsigned int offset = irq % 32;
	unsigned int enableRegister = GICD_ENABLE_IRQ_BASE + (4*n);
	printf("EnableRegister: %x\r\n", enableRegister);
	put32(enableRegister, 1 << offset);
}

void assign_target(unsigned int irq, __attribute__((unused))unsigned int cpu) {
	unsigned int n = irq / 4;
	unsigned int targetRegister = GIC_IRQ_TARGET_BASE + (4*n);
	 // Currently we only enter the target CPU 0
	put32(targetRegister, get32(targetRegister) | (1 << 8));
}

void enable_interrupt_controller()
{
	switch (pi_ver) {
		case 3:
			irqs.system_timer.irq0 = (1 << 0);
			irqs.system_timer.irq1 = (1 << 1);
			irqs.system_timer.irq2 = (1 << 2);
			irqs.system_timer.irq3 = (1 << 3);

			printf("Enabling interrupt %x\r\n", irqs.system_timer.irq1);
			put32(ENABLE_IRQS_1, irqs.system_timer.irq1);
			break;

		case 4:
		 	irqs.system_timer.irq0 = (0x60); //96
			irqs.system_timer.irq1 = (0x61); //97
			irqs.system_timer.irq2 = (0x62); //98
			irqs.system_timer.irq3 = (0x63); //99

			assign_target(irqs.system_timer.irq1, 0);
			enable_interrupt(irqs.system_timer.irq1);
			break;
	}
}

void show_invalid_entry_message(int type, uint64_t esr, uint64_t elr, uint64_t spsr, int el) {
	printf("Invalid interrupt: %s (type %d), ESR: 0x%08x, ELR: 0x%016x, SPSR: 0x%08X, EL: %d\r\n", entry_error_messages[type], type, esr, elr, spsr, el);
}

void handle_irq(void)
{
	unsigned int irq;
	unsigned int irq_ack_reg;

	switch (pi_ver) {
		case 3:
			irq = get32(IRQ_PENDING_1);
			break;

		case 4:
			//irq = get32(IRQ_BASIC_PENDING);
			irq_ack_reg = get32(GICC_IAR);
			irq = irq_ack_reg & 0x2FF;
			break;
	}
	//printf("Got pending interrupt: %x\r\n", irq);

	if (irq == irqs.system_timer.irq1) {
		if (pi_ver == 4) {
			put32(GICC_EOIR, irq_ack_reg);
		}
		handle_timer_irq();
	} else {
		printf("WARNING: unhandled interrupt: %x\r\n", irq);
	}
}