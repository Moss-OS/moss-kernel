#pragma once

#include "peripherals/base.h"

#define VA_START 				0xffff000000000000

#define PHYS_MEMORY_SIZE 		0x40000000

#define PAGE_MASK				0xfffffffffffff000
#define PAGE_SHIFT	 			12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)

#define LOW_MEMORY              (2 * SECTION_SIZE)

#define PI3_HIGH_MEMORY         DEVICE_BASE
#define PI3_PAGING_MEMORY       (PI3_HIGH_MEMORY - LOW_MEMORY)
#define PI3_PAGING_PAGES        (PI3_PAGING_MEMORY/PAGE_SIZE)
#define PI4_HIGH_MEMORY         0xFE000000 // TODO_FIX_PI4
#define PI4_PAGING_MEMORY       (PI4_HIGH_MEMORY - LOW_MEMORY)
#define PI4_PAGING_PAGES        (PI4_PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

#define PGD_SHIFT				PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT				PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT				PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE				(3 * PAGE_SIZE)

#ifndef __ASSEMBLER__
#include "process/sched.h"
void init_mem_map();

uint64_t get_free_page();
void free_page(uint64_t p);
void map_page(struct task_struct *task, uint64_t va, uint64_t page);
void memzero(uint64_t src, uint64_t n);
void memcpy(uint64_t dst, uint64_t src, uint64_t n);

int copy_virt_memory(struct task_struct *dst);
uint64_t allocate_kernel_page();
uint64_t allocate_user_page(struct task_struct *task, uint64_t va);

extern uint64_t pg_dir;

#endif