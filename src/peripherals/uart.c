#include "uart.h"
#include "../common/utils.h"

void uart_init (void)
{
	unsigned int selector;
	selector = get32(GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 4<<12;                      // set alt0 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 4<<15;                      // set alt0 for gpio 15
	put32(GPFSEL1, selector);

	put32(GPPUD, 0);
	delay(150);
	put32(GPPUDCLK0, (1<<14) | (1<<15));
	delay(150);
	put32(GPPUDCLK0, 0);

	put32(UART0_CR, 0);                    // disable UART until configuration is done
	/* baud divisor = UARTCLK / (16 * baud_rate) = 48 * 10^6 / (16 * 115200) = 26.0416666667
     * integer part = 26
	 * fractional part = (int) ((0.0416666667 * 64) + 0.5) = 3
	 * generated baud rate divisor = 26 + (3 / 64) = 26.046875
     * generated baud rate = (48 * 10^6) / (16 * 26.046875) = 115177
	 * error = |(115177 - 115200) / 115200 * 100| = 0.02%
	 */
	put32(UART0_IBRD, 26);                 // baud rate divisor, integer part
	put32(UART0_FBRD, 3);                  // baud rate divisor, fractional part
	put32(UART0_LCRH, (1<<4) | (3<<5));    // enable FIFOs and 8 bits frames
	put32(UART0_IMSC, 0);                  // mask interupts
	put32(UART0_CR, 1 | (1<<8) | (1<<9));  // enable UART, receive and transmit
}

void uart_send (unsigned char c)
{
	// wait for transmit FIFO to have an available slot
	while(get32(UART0_FR) & (1<<5)) { }
	put32(UART0_DR, c);
}

unsigned char uart_recv ()
{
	// wait for receive FIFO to have data to read
	while(get32(UART0_FR) & (1<<4)) { }
	return(get32(UART0_DR) & 0xFF);
}

void uart_send_string(const char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char) str[i]);
	}
}

