#pragma once

#define PAGE_SHIFT	 			12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)

#define LOW_MEMORY              (2 * SECTION_SIZE)

#define PI3_HIGH_MEMORY         0x3F000000
#define PI3_PAGING_MEMORY       (PI3_HIGH_MEMORY - LOW_MEMORY)
#define PI3_PAGING_PAGES        (PI3_PAGING_MEMORY/PAGE_SIZE)
#define PI4_HIGH_MEMORY         0xFE000000
#define PI4_PAGING_MEMORY       (PI4_HIGH_MEMORY - LOW_MEMORY)
#define PI4_PAGING_PAGES        (PI4_PAGING_MEMORY/PAGE_SIZE)

#ifndef __ASSEMBLER__

void init_mem_map();
uint64_t get_free_page();
void free_page(uint64_t p);
void memzero(uint64_t src, uint64_t n);

#endif