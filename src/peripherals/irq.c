#include "../common/utils.h"
#include "../common/printf.h"
#include "timer.h"
#include "../kernel/entry.h"
#include "irq.h"

const char *entry_error_messages[] = {
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
	"ERROR_INVALID_EL0_32"
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
	switch (get_raspi_ver()) {
		case RASPI3:
			SYSTEM_TIMER_IRQ_0 = (1 << 0);
			SYSTEM_TIMER_IRQ_1 = (1 << 1);
			SYSTEM_TIMER_IRQ_2 = (1 << 2);
			SYSTEM_TIMER_IRQ_3 = (1 << 3);

			printf("Enabling interrupt %x\r\n", SYSTEM_TIMER_IRQ_1);
			put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
			break;

		case RASPI4:
		 	SYSTEM_TIMER_IRQ_0 = (0x60); //96
			SYSTEM_TIMER_IRQ_1 = (0x61); //97
			SYSTEM_TIMER_IRQ_2 = (0x62); //98
			SYSTEM_TIMER_IRQ_3 = (0x63); //99

			assign_target(SYSTEM_TIMER_IRQ_1, 0);
			enable_interrupt(SYSTEM_TIMER_IRQ_1);
			break;

		default:
			return;
			// die?
	}
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	printf("%s, %d, ESR: %x, address: %x\r\n", entry_error_messages[type], type, esr, address);
}

void handle_irq(void)
{
	unsigned int irq;
	unsigned int irq_ack_reg;

	switch (get_raspi_ver()) {
		case RASPI3:
			irq = get32(IRQ_PENDING_1);
			break;

		case RASPI4:
			//irq = get32(IRQ_BASIC_PENDING);
			irq_ack_reg = get32(GICC_IAR);
			irq = irq_ack_reg & 0x2FF;
			break;

		default:
			return;
			// die?
	}
	printf("Got pending irq: %x\r\n", irq);

	if (irq == SYSTEM_TIMER_IRQ_1) {
		if (get_raspi_ver() == RASPI4)
			put32(GICC_EOIR, irq_ack_reg);
		handle_timer_irq();
	} else {
		printf("Unknown pending irq: %x\r\n", irq);
	}
}