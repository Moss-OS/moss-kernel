#include "../peripherals/uart.h"

static unsigned int current_processor_index = 0;

void kernel_main(unsigned long processor_index)
{

	if (processor_index == 0) {
		uart_init();
	}

	while (processor_index != current_processor_index) { }

	uart_send_string("Hello from processor ");
	uart_send(processor_index + '0');
	uart_send_string("!\r\n");

	// Tell the next core to go
	current_processor_index++;

	// Only core 0 do the echo
	if (processor_index == 0) {
		// if current_processor_index == 4 then all processors are done
		while (current_processor_index != 4) { }

		while (1) {
			uart_send(uart_recv());
		}
	}
}