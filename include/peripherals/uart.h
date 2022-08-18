#pragma once

#include <stddef.h>
#include <stdint.h>

// GPIO
struct {
	// The GPIO registers base address.
	unsigned int	base;
	unsigned int	gpfsel1;
	unsigned int	gpset0;
	unsigned int	gpclr0;
	unsigned int	gppud;
	unsigned int	gppudclk0;
} gpio_regs;

struct {
	// The base address for UART.
	unsigned int	base;
	unsigned int	dr;
	unsigned int    rsrecr;
	unsigned int    fr;
	unsigned int    ilpr;
	unsigned int    ibrd;
	unsigned int    fbrd;
	unsigned int    lcrh;
	unsigned int    cr;
	unsigned int    ifls;
	unsigned int    imsc;
	unsigned int    ris;
	unsigned int    mis;
	unsigned int    icr;
	unsigned int    dmacr;
	unsigned int    itcr;
	unsigned int    itip;
	unsigned int    itop;
	unsigned int    tdr;
} uart0_regs;

void uart_init();

void uart_send(unsigned char c);
unsigned char uart_recv();

void uart_send_string(const char* str);