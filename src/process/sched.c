#include <stddef.h>
#include "process/sched.h"
#include "peripherals/irq.h"
#include "common/printf.h"

static struct task_struct init_task = INIT_TASK;

void init_scheduler()
{
	current = &(init_task);
	for (int i = 0; i < NR_TASKS; i++) {
		task[i] = &(init_task);
	}
	nr_tasks = 1;

}

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	//printf("in _schedule\r\n");
	//print_task_info(current);
	preempt_disable();
	int next,c;
	struct task_struct * p;
	//printf("Entering _schedule loop\r\n");
	while (1) {
		//printf(".");
		c = -1;
		next = 0;
	 	//printf("Entering _schedule first for\r\n");
		for (int i = 0; i < nr_tasks; i++){
			p = task[i];
			//print_task_info(p);
			//printf("c = %d / next = %d\r\n", c, next);
			if (p && p->state == TASK_RUNNING && p->counter > c) {
	 			//printf("in _schedule first if\r\n");
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
            p = task[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
	}
	//printf("Attempting to switch to task %d\r\n", next);
	switch_to(task[next]);
	preempt_enable();
}

void schedule(void)
{
	//printf("in schedule\r\n", NULL);
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next)
{
	//printf("task output: ");
	if (current == next)
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}


void timer_tick()
{
	//print_task_info(current);
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	//print_task_info(current);
	enable_irq();
	_schedule();
	disable_irq();
}

void print_task_info(struct task_struct *p)
{
	printf("Task id: %d - ", p->id);
	printf("state: %d ", p->state);
	printf("counter: %d ", p->counter);
	printf("priority: %d ", p->priority);
	printf("preempt_count: %d ", p->preempt_count);
	printf(" - located at 0x%08X - ", p);
	printf("x19(fn) 0x%08X / ", p->cpu_context.x19);
	printf("x20(arg) 0x%08X / ", p->cpu_context.x20);
	printf("fp 0x%08X / ", p->cpu_context.fp);
	printf("sp 0x%08X / ", p->cpu_context.sp);
	printf("pc 0x%08X)\r\n", p->cpu_context.pc);
}