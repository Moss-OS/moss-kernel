#include "peripherals/uart.h"
#include "common/utils.h"

void uart_init (void)
{
	switch (get_raspi_ver()) {
		case RASPI3:
			GPIO_BASE = 0x3f200000;   // raspi2 & 3
			break;

		case RASPI4:
			GPIO_BASE = 0xFE200000;   // raspi4
			break;

		default:
			return;
			// die?
	}

	// GPIO
	GPFSEL1		= (GPIO_BASE + 0x04);
	GPSET0		= (GPIO_BASE + 0x1C);
	GPCLR0		= (GPIO_BASE + 0x28);
	GPPUD		= (GPIO_BASE + 0x94);
	GPPUDCLK0	= (GPIO_BASE + 0x98);

	// The base address for UART.
	UART0_BASE   = (GPIO_BASE + 0x1000);
	UART0_DR     = (UART0_BASE + 0x00);
	UART0_RSRECR = (UART0_BASE + 0x04);
	UART0_FR     = (UART0_BASE + 0x18);
    UART0_ILPR   = (UART0_BASE + 0x20);
    UART0_IBRD   = (UART0_BASE + 0x24);
    UART0_FBRD   = (UART0_BASE + 0x28);
    UART0_LCRH   = (UART0_BASE + 0x2C);
    UART0_CR     = (UART0_BASE + 0x30);
    UART0_IFLS   = (UART0_BASE + 0x34);
    UART0_IMSC   = (UART0_BASE + 0x38);
    UART0_RIS    = (UART0_BASE + 0x3C);
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44);
    UART0_DMACR  = (UART0_BASE + 0x48);
    UART0_ITCR   = (UART0_BASE + 0x80);
    UART0_ITIP   = (UART0_BASE + 0x84);
    UART0_ITOP   = (UART0_BASE + 0x88);
    UART0_TDR    = (UART0_BASE + 0x8C);

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