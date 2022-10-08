#include "process/fork.h"
#include "common/printf.h"
#include "common/utils.h"
#include "process/sched.h"
#include "bootstrapper/mm.h"

void sys_write(char * buf) {
	printf(buf);
}

int sys_clone(uint64_t stack) {
	return copy_process(0, 0, 0, stack);
}

uint64_t sys_malloc() {
	uint64_t addr = get_free_page();
	if (!addr) {
		return -1;
	}
	return addr;
}

void sys_exit() {
	exit_process();
}

void * const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit};