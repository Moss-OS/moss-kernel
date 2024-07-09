#include <stdint.h>
#include <stddef.h>
#include "mini_uart.h"
#include "stdio.h"
#include "stdlib.h"

char getc() {
    return uart_recv();
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