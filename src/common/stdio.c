#include <stdint.h>
#include <stddef.h>
#include "peripherals/mini_uart.h"
#include "peripherals/uart.h"
#include "common/pi.h"
#include "stdio.h"
#include "stdlib.h"

char getc() {
    if (pi_ver == 4) return (char)uart_recv();
    return mini_uart_recv();
}

void putc ( void* p, char c)
{
	if (pi_ver == 4) uart_send((unsigned char)c);
	else mini_uart_send(c);
}

void puts(const char * str) {
    int i;
    for (i = 0; str[i] != '\0'; i ++)
        putc(NULL, str[i]);
}

void gets(char * buf, int buflen) {
    int i;
    char c;
    // Leave a spot for null char in buffer
    for (i = 0; (c = getc()) != '\r' && buflen > 1; i++, buflen--) {
        putc(NULL, c);
        buf[i] = c;
    }

    putc(NULL, '\n');
    if (c == '\n') {
        buf[i] = '\0';
    }
    else
        buf[buflen-1] = '\0';
}