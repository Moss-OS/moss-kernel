#include <stdint.h>
#include "peripherals/uart.h"
#include "common/utils.h"
#include "common/pi.h"

struct _gpio_regs gpio_regs;
struct _uart0_regs uart0_regs;

void uart_init (void)
{
	switch (pi_ver) {
		case 3:
			gpio_regs.base = 0x3f200000;   // pi2 & 3
			break;

		case 4:
			gpio_regs.base = 0xFE200000;   // pi4
			break;
	}

	// GPIO
	gpio_regs.gpfsel1		= (gpio_regs.base + 0x04);
	gpio_regs.gpset0		= (gpio_regs.base + 0x1C);
	gpio_regs.gpclr0		= (gpio_regs.base + 0x28);
	gpio_regs.gppud			= (gpio_regs.base + 0x94);
	gpio_regs.gppudclk0		= (gpio_regs.base + 0x98);

	// The base address for UART.
	uart0_regs.base   = (gpio_regs.base + 0x1000);
	uart0_regs.dr     = (uart0_regs.base + 0x00);
	uart0_regs.rsrecr = (uart0_regs.base + 0x04);
	uart0_regs.fr     = (uart0_regs.base + 0x18);
    uart0_regs.ilpr   = (uart0_regs.base + 0x20);
    uart0_regs.ibrd   = (uart0_regs.base + 0x24);
    uart0_regs.fbrd   = (uart0_regs.base + 0x28);
    uart0_regs.lcrh   = (uart0_regs.base + 0x2C);
    uart0_regs.cr     = (uart0_regs.base + 0x30);
    uart0_regs.ifls   = (uart0_regs.base + 0x34);
    uart0_regs.imsc   = (uart0_regs.base + 0x38);
    uart0_regs.ris    = (uart0_regs.base + 0x3C);
    uart0_regs.mis    = (uart0_regs.base + 0x40),
    uart0_regs.icr    = (uart0_regs.base + 0x44);
    uart0_regs.dmacr  = (uart0_regs.base + 0x48);
    uart0_regs.itcr   = (uart0_regs.base + 0x80);
    uart0_regs.itip   = (uart0_regs.base + 0x84);
    uart0_regs.itop   = (uart0_regs.base + 0x88);
    uart0_regs.tdr    = (uart0_regs.base + 0x8C);

	uint32_t selector;
	selector = get32(gpio_regs.gpfsel1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 4<<12;                      // set alt0 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 4<<15;                      // set alt0 for gpio 15
	put32(gpio_regs.gpfsel1, selector);

	put32(gpio_regs.gppud, 0);
	delay(150);
	put32(gpio_regs.gppudclk0, (1<<14) | (1<<15));
	delay(150);
	put32(gpio_regs.gppudclk0, 0);

	put32(uart0_regs.cr, 0);                    // disable UART until configuration is done
	/* baud divisor = UARTCLK / (16 * baud_rate) = 48 * 10^6 / (16 * 115200) = 26.0416666667
     * integer part = 26
	 * fractional part = (int) ((0.0416666667 * 64) + 0.5) = 3
	 * generated baud rate divisor = 26 + (3 / 64) = 26.046875
     * generated baud rate = (48 * 10^6) / (16 * 26.046875) = 115177
	 * error = |(115177 - 115200) / 115200 * 100| = 0.02%
	 */
	put32(uart0_regs.ibrd, 26);                 // baud rate divisor, integer part
	put32(uart0_regs.fbrd, 3);                  // baud rate divisor, fractional part
	put32(uart0_regs.lcrh, (1<<4) | (3<<5));    // enable FIFOs and 8 bits frames
	put32(uart0_regs.imsc, 0);                  // mask interupts
	put32(uart0_regs.cr, 1 | (1<<8) | (1<<9));  // enable UART, receive and transmit
}

void uart_send (unsigned char c)
{
	// wait for transmit FIFO to have an available slot
	while(get32(uart0_regs.fr) & (1<<5)) { }
	put32(uart0_regs.dr, c);
}

unsigned char uart_recv ()
{
	// wait for receive FIFO to have data to read
	while(get32(uart0_regs.fr) & (1<<4)) { }
	return(get32(uart0_regs.dr) & 0xFF);
}

void uart_send_string(const char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char) str[i]);
	}
}