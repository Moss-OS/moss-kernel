#ifndef	_TIMER_H
#define	_TIMER_H

unsigned int TIMER_BASE;

unsigned int TIMER_CS;
unsigned int TIMER_CLO;
unsigned int TIMER_CHI;
unsigned int TIMER_C0;
unsigned int TIMER_C1;
unsigned int TIMER_C2;
unsigned int TIMER_C3;

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

void timer_init ( void );
void handle_timer_irq ( void );

#endif  /*_TIMER_H */