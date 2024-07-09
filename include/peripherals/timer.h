#pragma once

struct _timer_regs {
	uint64_t base;
	uint64_t control_status;
	uint64_t counter_low;
	uint64_t counter_high;
	uint64_t compare[4];
};

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

void timer_init (void);
void handle_timer_irq (void);