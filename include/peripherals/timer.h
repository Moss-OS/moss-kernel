#pragma once


struct {
	unsigned int base;
	unsigned int control_status;
	unsigned int counter_low;
	unsigned int counter_high;
	unsigned int compare[4];
} timer_regs;

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

void timer_init ( void );
void handle_timer_irq ( void );