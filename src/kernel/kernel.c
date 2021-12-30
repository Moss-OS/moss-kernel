#include <stdint.h>
#include "../peripherals/uart.h"

void kernel_main(uint64_t r0, uint64_t r1, uint64_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");

    while (1) {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}