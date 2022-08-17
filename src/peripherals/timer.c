#include "../common/utils.h"
#include "../common/printf.h"
#include "timer.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init ( void )
{
	switch (get_raspi_ver()) {
		case (RASPI3):
			TIMER_BASE = 0x3F000000;
			break;
		case (RASPI4):
			TIMER_BASE = 0xFE000000;
			break;
		default:
			return;
			// die?
	}
	TIMER_CS = (TIMER_BASE+0x00003000);
	TIMER_CLO = (TIMER_BASE+0x00003004);
	TIMER_CHI = (TIMER_BASE+0x00003008);
	TIMER_C0 = (TIMER_BASE+0x0000300C);
	TIMER_C1 = (TIMER_BASE+0x00003010);
	TIMER_C2 = (TIMER_BASE+0x00003014);
	TIMER_C3 = (TIMER_BASE+0x00003018);

	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void )
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	printf("Timer interrupt received\n\r");
}