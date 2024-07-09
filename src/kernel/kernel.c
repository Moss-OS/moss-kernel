#include <stdint.h>
#include "mini_uart.h"
#include "common/utils.h"
#include "common/pi.h"
#include "stdio.h"
#include "peripherals/timer.h"
#include "peripherals/irq.h"
#include "process/sched.h"
#include "process/fork.h"
#include "kernel/sys.h"
#include "process/user.h"


void kernel_process() {
	printf("Kernel process started. EL %d\r\n", get_el());
	uint64_t begin = (uint64_t)&user_begin;
	uint64_t end = (uint64_t)&user_end;
	uint64_t process = (uint64_t)&user_process;
	int err = move_to_user_mode(begin, end - begin, process - begin);
	if (err < 0) {
		printf("Error while moving process to user mode\r\n");
	}
}

static uint32_t current_processor_index = 0;

void kernel_main(uint64_t processor_index) {

	if (processor_index == 1) {
		pi_ver = set_pi_ver();
		uart_init();
		init_printf("", putc);

		printf("\r\n");
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n");
		printf("                    Welcome to MossOS!!!\r\n");
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n");
		printf("\r\n");
		printf("Moss kernel initializing\r\n");
		printf("Raspberry Pi version: %d\r\n", pi_ver);
		printf("Exception level: %d \r\n", get_el());
		printf("Running on core %d\r\n", processor_index);
		//printf("Starting all cores\r\n");
	}

	//while (processor_index != current_processor_index) { }

	//printf("Hello from processor %d! My exception level is %d\r\n", processor_index, get_el());

	// Tell the next core to go
	current_processor_index++;

	// Only core 0 do the echo
	//if (processor_index == 0) {
		// if current_processor_index == 4 then all processors are done
		//while (current_processor_index != 4) { }

		printf("Initializing memory map\r\n");
		init_mem_map();
		printf("Initializing scheduler\r\n");
		init_scheduler();
		printf("Initializing interrupts\r\n");
		irq_vector_init();
		printf("Initializing timer\r\n");
		timer_init();
		printf("Enable interrupt controller\r\n");
		enable_interrupt_controller();
		printf("Enable interrupts\r\n");
		enable_irq();

		int res = copy_process(PF_KTHREAD, (uint64_t)&kernel_process, 0);
		if (res < 0) {
			printf("Error while starting kernel process\r\n");
			return;
		}

		printf("Entering kernel loop\r\n");
		while (1) {
			schedule();
		}
	//}
}