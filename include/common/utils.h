#pragma once

#ifndef __ASSEMBLER__
extern uint32_t get_core_info(void);
extern uint32_t get_core_id(void);
extern uint32_t get_el(void);

extern void put32(uint64_t, uint32_t);
extern uint32_t get32(uint64_t);

extern void delay(uint64_t);
#endif