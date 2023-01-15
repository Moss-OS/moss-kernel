#pragma once

// GPIO
struct _gpio_regs {
	// The GPIO registers base address.
	uint32_t	base;
	uint32_t	gpfsel1;
	uint32_t	gpset0;
	uint32_t	gpclr0;
	uint32_t	gppud;
	uint32_t	gppudclk0;
};

struct _uart0_regs {
	// The base address for UART.
	uint32_t	base;
	uint32_t	dr;
	uint32_t    rsrecr;
	uint32_t    fr;
	uint32_t    ilpr;
	uint32_t    ibrd;
	uint32_t    fbrd;
	uint32_t    lcrh;
	uint32_t    cr;
	uint32_t    ifls;
	uint32_t    imsc;
	uint32_t    ris;
	uint32_t    mis;
	uint32_t    icr;
	uint32_t    dmacr;
	uint32_t    itcr;
	uint32_t    itip;
	uint32_t    itop;
	uint32_t    tdr;
};

void uart_init();

void uart_send(unsigned char c);
unsigned char uart_recv();

void uart_send_string(const char* str);