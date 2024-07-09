#pragma once
#include <stdint.h>

void call_sys_write(char * buf);
int call_sys_fork();
void call_sys_exit();

extern void user_delay(uint64_t);
extern uint64_t get_sp(void);
extern uint64_t get_pc(void);