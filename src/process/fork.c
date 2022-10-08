#include <stdint.h>
#include "process/fork.h"
#include "process/sched.h"
#include "bootstrapper/mm.h"
#include "kernel/entry.h"
#include "common/printf.h"


int copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg, uint64_t stack) {
	preempt_disable();
	struct task_struct *p;
	struct task_struct *previous_task;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return 1;

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((uint64_t)childregs, sizeof(struct pt_regs));
	memzero((uint64_t)&p->cpu_context, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs * cur_regs = task_pt_regs(current);
		*childregs = *cur_regs;
		childregs->regs[0] = 0;
		childregs->sp = stack + PAGE_SIZE;
		p->stack = stack;
	}
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail

	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	p->cpu_context.pc = (uint64_t)ret_from_fork;
	p->cpu_context.sp = (uint64_t)childregs;
	int pid = nr_tasks++;
	p->id = pid;

	p->next_task = 0;
	previous_task = task_list;
	while(previous_task->next_task) previous_task = previous_task->next_task;
	previous_task->next_task = p;

	printf("Struct task allocated at 0x%08X.\r\n", p);
	print_task_info(p);

	preempt_enable();
	return pid;
}

int move_to_user_mode(uint64_t pc) {
	struct pt_regs *regs = task_pt_regs(current);
	memzero((uint64_t)regs, sizeof(*regs));
	regs->pc = pc;
	regs->pstate = PSR_MODE_EL0t;
	uint64_t stack = get_free_page(); //allocate new user stack
	if (!stack) {
		return -1;
	}
	regs->sp = stack + PAGE_SIZE;
	current->stack = stack;
	printf("PC 0x%08X, PSTATE 0x%08X, REGS 0x%08X, SP 0x%08X\r\n",regs->pc, regs->pstate, regs->regs, regs->sp);
	return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk) {
	uint64_t p = (uint64_t)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}