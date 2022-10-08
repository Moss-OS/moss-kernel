#pragma once


struct {
	uint32_t base;
	uint32_t control_status;
	uint32_t counter_low;
	uint32_t counter_high;
	uint32_t compare[4];
} timer_regs;

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

void timer_init ( void );
void handle_timer_irq ( void );