#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

#define PI3_DEVICE_BASE         0x3F000000UL
#define PI4_DEVICE_BASE         0xFE000000UL

#ifndef __ASSEMBLER__
extern uint64_t device_base;
extern uint64_t pbase;

void init_peripheral_bases(void);
#endif

#define PBASE                   (pbase)