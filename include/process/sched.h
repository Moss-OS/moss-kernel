#pragma once

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct

#ifndef __ASSEMBLER__

#define THREAD_SIZE					4096
#define TASK_RUNNING				0
#define TASK_ZOMBIE				1

#define PF_KTHREAD		            	0x00000002

extern struct task_struct *current;
extern struct task_struct *task_list;
extern int nr_tasks;

struct cpu_context {
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t fp;
	uint64_t sp;
	uint64_t pc;
};

struct task_struct {
	struct cpu_context cpu_context;
	int id;
	long state;
	long counter;
	long priority;
	long preempt_count;
	long stack;
	uint64_t flags;
	struct task_struct *next_task;
};

void init_scheduler(void);
void schedule(void);
void timer_tick(void);
void preempt_disable(void);
void preempt_enable(void);
void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process(void);
void print_task_info(struct task_struct *p);

#define INIT_TASK {\
	{/*cpu cotext */\
		0,0,0,0,0,0,0,0,0,0, /* x19 - x28 */\
		0, /* fp */\
		0, /* sp */\
		0 /* pc */\
	},\
	0, /* id */\
	0, /* state */\
	0, /* counter */\
	1, /* priority */\
	0, /* prempt_cout */\
	0, /* stack */\
	PF_KTHREAD, /* flags */\
	0  /* *next_task */\
}

#endif