#include "bootstrapper/mm.h"
#include "common/pi.h"

uint16_t *mem_map;

void init_mem_map()
{
	int pages;
	switch(pi_ver) {
		case 3: ;
			uint16_t mm3[PI3_PAGING_PAGES];
			pages = PI3_PAGING_PAGES;
			mem_map = mm3;
			break;
		case 4: ;
			uint16_t mm4[PI4_PAGING_PAGES];
			pages = PI4_PAGING_PAGES;
			mem_map = mm4;
			break;
	}
	for (int i = 0; i < pages; i++){
		mem_map[i] = 0;
	}
}

uint64_t get_free_page()
{
	int pages = PI3_PAGING_PAGES;
	for (int i = 0; i < pages; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			return LOW_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(uint64_t p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}