#pragma once
#include <stdint.h>

// GPIO
struct _gpio_regs {
	// The GPIO registers base address.
	uint64_t	base;
	uint64_t	gpfsel1;
	uint64_t	gpset0;
	uint64_t	gpclr0;
	uint64_t	gppud;
	uint64_t	gppudclk0;
};

struct _uart0_regs {
	// The base address for UART.
	uint64_t	base;
	uint64_t	dr;
	uint64_t	rsrecr;
	uint64_t	fr;
	uint64_t	ilpr;
	uint64_t	ibrd;
	uint64_t	fbrd;
	uint64_t	lcrh;
	uint64_t	cr;
	uint64_t	ifls;
	uint64_t	imsc;
	uint64_t	ris;
	uint64_t	mis;
	uint64_t	icr;
	uint64_t	dmacr;
	uint64_t	itcr;
	uint64_t	itip;
	uint64_t	itop;
	uint64_t	tdr;
};

void uart_init();

void uart_send(unsigned char c);
unsigned char uart_recv();

void uart_send_string(const char* str);