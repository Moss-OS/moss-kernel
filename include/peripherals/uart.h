#pragma once

#include <stddef.h>
#include <stdint.h>

// The GPIO registers base address.
unsigned int	GPIO_BASE;

// GPIO
unsigned int	GPFSEL1;
unsigned int	GPSET0;
unsigned int	GPCLR0;
unsigned int	GPPUD;
unsigned int	GPPUDCLK0;

// The base address for UART.
unsigned int	UART0_BASE;
unsigned int	UART0_DR;
unsigned int    UART0_RSRECR;
unsigned int    UART0_FR;
unsigned int    UART0_ILPR;
unsigned int    UART0_IBRD;
unsigned int    UART0_FBRD;
unsigned int    UART0_LCRH;
unsigned int    UART0_CR;
unsigned int    UART0_IFLS;
unsigned int    UART0_IMSC;
unsigned int    UART0_RIS;
unsigned int    UART0_MIS;
unsigned int    UART0_ICR;
unsigned int    UART0_DMACR;
unsigned int    UART0_ITCR;
unsigned int    UART0_ITIP;
unsigned int    UART0_ITOP;
unsigned int    UART0_TDR;

void uart_init();

void uart_send(unsigned char c);
unsigned char uart_recv();

void uart_send_string(const char* str);