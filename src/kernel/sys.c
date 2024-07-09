#include <stdint.h>
#include "process/fork.h"
#include "common/printf.h"
#include "common/utils.h"
#include "process/sched.h"
#include "bootstrapper/mm.h"

void sys_write(char * buf){
	printf(buf);
}

int sys_fork() {
	return copy_process(0, 0, 0);
}

void sys_exit() {
	exit_process();
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit};