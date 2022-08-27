#include "peripherals/uart.h"
#include "common/utils.h"
#include "common/pi.h"
#include "stdio.h"
#include "peripherals/timer.h"
#include "peripherals/irq.h"
#include "process/sched.h"
#include "process/fork.h"

void process(char *array)
{
	while (1){
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(100000);
		}
	}
}

static unsigned int current_processor_index = 0;

void kernel_main(unsigned long processor_index)
{

	if (processor_index == 0) {
		pi_ver = set_pi_ver();
		uart_init();
		init_printf(NULL, putc);

		printf("\r\n");
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n");
		printf("                    Welcome to MossOS!!!\r\n", NULL);
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n");
		printf("\r\n");
		printf("Moss kernel initializing\r\n");
		printf("Raspberry Pi version: %d\r\n", pi_ver);
		printf("Exception level: %d \r\n", get_el());
		printf("Starting all cores\r\n");
	}

	while (processor_index != current_processor_index) { }

	printf("Hello from processor %d! My exception level is %d\r\n", processor_index, get_el());

	// Tell the next core to go
	current_processor_index++;

	// Only core 0 do the echo
	if (processor_index == 0) {
		// if current_processor_index == 4 then all processors are done
		while (current_processor_index != 4) { }

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

		struct task_struct *p = current;
		printf("Kernel process %d\r\n", nr_tasks-1);
		print_task_info(p);
		printf("Starting process 1\r\n");
		int res = copy_process((unsigned long)&process, (unsigned long)"12345");
		if (res != 0) {
			printf("Error while starting process 1\r\n");
			return;
		}
		printf("Starting process 2\r\n");
		res = copy_process((unsigned long)&process, (unsigned long)"abcde");
		if (res != 0) {
			printf("Error while starting process 2\r\n");
			return;
		}
		printf("Starting process 3\r\n");
		res = copy_process((unsigned long)&process, (unsigned long)"qwerty");
		if (res != 0) {
			printf("Error while starting process 2\r\n");
			return;
		}

		printf("Entering kernel loop\r\n");

		while (1) {
			//printf("in kernel loop\r\n");
			schedule();
		}
	}
}