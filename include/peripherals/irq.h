#pragma once

// Pi3
#define IRQ_BASE 0x3F000000
#define IRQ_BASIC_PENDING	(IRQ_BASE+0x0000B200)
#define IRQ_PENDING_1		(IRQ_BASE+0x0000B204)
#define IRQ_PENDING_2		(IRQ_BASE+0x0000B208)
#define FIQ_CONTROL			(IRQ_BASE+0x0000B20C)
#define ENABLE_IRQS_1		(IRQ_BASE+0x0000B210)
#define ENABLE_IRQS_2		(IRQ_BASE+0x0000B214)
#define ENABLE_BASIC_IRQS	(IRQ_BASE+0x0000B218)
#define DISABLE_IRQS_1		(IRQ_BASE+0x0000B21C)
#define DISABLE_IRQS_2		(IRQ_BASE+0x0000B220)
#define DISABLE_BASIC_IRQS	(IRQ_BASE+0x0000B224)

// Pi4 https://github.com/s-matyukevich/raspberry-pi-os/issues/237
#define GIC_BASE 				0xFF840000
#define GICD_DIST_BASE 			(GIC_BASE+0x00001000)
#define GICC_CPU_BASE 			(GIC_BASE+0x00002000)
#define GICD_ENABLE_IRQ_BASE 	(GICD_DIST_BASE+0x00000100)
#define GICC_IAR 				(GICC_CPU_BASE+0x0000000C)
#define GICC_EOIR 				(GICC_CPU_BASE+0x00000010)
#define GIC_IRQ_TARGET_BASE 	(GICD_DIST_BASE+0x00000800)

struct {
	// Timer interrupts
	struct {
		uint32_t irq0;
		uint32_t irq1;
		uint32_t irq2;
		uint32_t irq3;
	} system_timer;
} irqs;

// Functions
void enable_interrupt_controller(void);

void irq_vector_init(void);
void enable_irq(void);
void disable_irq(void);