#include "../peripherals/uart.h"
#include "../common/utils.h"
#include "../common/stdio.h"
#include "../peripherals/timer.h"
#include "../peripherals/irq.h"

static unsigned int current_processor_index = 0;

void kernel_main(unsigned long processor_index)
{

	if (processor_index == 0) {
		uart_init();
		init_printf(NULL, putc);

		printf("\n", NULL);
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n", NULL);
		printf("                    Welcome to MossOS!!!\r\n", NULL);
		printf("~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~\r\n", NULL);
		printf("\n", NULL);

		printf("Moss kernal initializing...\r\n", NULL);
		printf("Raspi version: %d\r\n", get_raspi_ver());
		printf("Exception level: %d \r\n", get_el());
		printf("\n", NULL);
	}

	while (processor_index != current_processor_index) { }

	printf("Hello from processor %d! ", processor_index);
	printf("My el is %d. \r\n", get_el());

	// Tell the next core to go
	current_processor_index++;

	// Only core 0 do the echo
	if (processor_index == 0) {
		// if current_processor_index == 4 then all processors are done
		while (current_processor_index != 4) { }

		// Test interrupts by running a system timer
		irq_vector_init();
		timer_init();
		enable_interrupt_controller();
		enable_irq();

		while (1) {
			uart_send(uart_recv());
		}
	}
}