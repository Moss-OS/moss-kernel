#include "common/utils.h"
#include "common/pi.h"
#include "common/printf.h"
#include "peripherals/timer.h"
#include "process/sched.h"

const uint32_t interval = 200000;
uint32_t curVal = 0;

void timer_init ( void )
{
	switch (pi_ver) {
		case 3:
			timer_regs.base = 0x3F000000;
			break;
		case 4:
			timer_regs.base = 0xFE000000;
			break;
	}
	timer_regs.control_status	= (timer_regs.base+0x00003000);
	timer_regs.counter_low		= (timer_regs.base+0x00003004);
	timer_regs.counter_high		= (timer_regs.base+0x00003008);
	timer_regs.compare[0] 		= (timer_regs.base+0x0000300C);
	timer_regs.compare[1] 		= (timer_regs.base+0x00003010);
	timer_regs.compare[2] 		= (timer_regs.base+0x00003014);
	timer_regs.compare[3] 		= (timer_regs.base+0x00003018);

	curVal = get32(timer_regs.counter_low);
	curVal += interval;
	put32(timer_regs.compare[1], curVal);
}

void handle_timer_irq( void )
{
	curVal += interval;
	put32(timer_regs.compare[1], curVal);
	put32(timer_regs.control_status, TIMER_CS_M1);
	//printf("Timer interrupt received\n\r");
	timer_tick();
}