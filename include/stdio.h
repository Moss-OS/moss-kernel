#ifndef _STDIO_H
#define _STDIO_H

#include "common/printf.h"

char getc(void);
void putc(void *p, char c);

void puts(const char * s);

// This version of gets copies until newline, replacing newline with null char, or until buflen.
// whichever comes first
void gets(char * buf, int buflen);

#endif /* _STDIO_H */