#pragma once

#ifndef __ASSEMBLER__
extern uint32_t get_core_info(void);
extern uint32_t get_core_id(void);
extern uint32_t get_el(void);

	extern void put32 ( unsigned long, unsigned int);
	extern unsigned int get32 ( unsigned long );

	extern void delay ( unsigned long );
#endif