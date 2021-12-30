#include <stdint.h>
#include "../common/stdio.h"
#include "../peripherals/uart.h"

void kernel_main(uint64_t r0, uint64_t r1, uint64_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    puts("Hello, kernel World!\r\n");

    while (1) {
        putc(getc());
        putc('\n');
    }
}