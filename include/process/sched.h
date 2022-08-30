#pragma once

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct

#ifndef __ASSEMBLER__

#define THREAD_SIZE					4096
#define TASK_RUNNING				0

struct task_struct *current;
struct task_struct *task_list;
int nr_tasks;

struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct task_struct {
	struct cpu_context cpu_context;
	int id;
	long state;
	long counter;
	long priority;
	long preempt_count;
	struct task_struct *next_task;
};

void init_scheduler(void);
void schedule(void);
void timer_tick(void);
void preempt_disable(void);
void preempt_enable(void);
void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void print_task_info(struct task_struct *p);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,0,1,0,0 \
}

#endif