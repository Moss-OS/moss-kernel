#include <stdint.h>
#include "../common/stdio.h"
#include "../common/utils.h"
#include "../peripherals/uart.h"

volatile unsigned int semaphore = 0;

void kernel_do(int id)
{
	// Wait for previous CPU to finish printing
	while(id != semaphore) { }

	puts("Hello, from processor ");
	putc(id + '0');
	puts("\r\n");

	// Tells the next CPU to go
	semaphore++;

    // Only CPU #0 do the echo
	if (id == 0) {
		// Wait for everyone else to finish
		while(semaphore != 4) { }

		while (1) {
			putc(getc());
		}
	}
}

void kernel_main(void) {
	uart_init();
	kernel_do(0);
}

void kernel_secondary(int core_id) {
	kernel_do(core_id);
}