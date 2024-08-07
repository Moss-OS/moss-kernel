#pragma once
#include <stdint.h>

#ifndef __ASSEMBLER__
extern uint32_t get_core_info(void);
extern uint32_t get_core_id(void);
extern uint64_t get_el(void);
extern void put32(uint64_t, uint32_t);
extern uint32_t get32(uint64_t);

extern void delay(uint64_t);

extern void set_pgd(uint64_t pgd);
extern uint64_t get_pgd();

#endif