/*
	MAX3100 SPI UART driver (for inbuilt peripheral SPI0)
*/
#include "platform.h"
#include "MAX3100.h"

enum E_MAX3100 {
	CONFIGURE = 0xC000, CONFIGURATION = 0x4000,
	SEND = 0x8000, READ = 0x0000, BAUD = 0x000F,
	TE_RA_FE = 0x0400, RTS_CTS = 0x0200, PARITY = 0x0100,
	TX_PENDING = 0x4000, RX_PENDING = 0x8000
};

// rounds down to the nearest usable baud rate, defaults to 230400
char MAX3100_baud(const long baud)
{
#define GET_BAUD_SETTING(o, n) if (baud >= o) return n;
	GET_BAUD_SETTING(230400,0)
	GET_BAUD_SETTING(115200,1)
	GET_BAUD_SETTING(76800, 2)
	GET_BAUD_SETTING(57600, 3)
	GET_BAUD_SETTING(38400, 4)
	GET_BAUD_SETTING(28800, 5)
	GET_BAUD_SETTING(19200, 6)
	GET_BAUD_SETTING(14400, 7)
	GET_BAUD_SETTING(9600,  8)
	GET_BAUD_SETTING(7200,  9)
	GET_BAUD_SETTING(4800, 10)
	GET_BAUD_SETTING(3600, 11)
	GET_BAUD_SETTING(2400, 12)
	GET_BAUD_SETTING(1800, 13)
	GET_BAUD_SETTING(1200, 14)
	GET_BAUD_SETTING(600,  15)
	return 0;
#undef GET_BAUD_SETTING
}

/* example word function for SPI0
unsigned int MAX3100_word(unsigned int out)
{
unsigned int in;
	SPI0_select(_MAX3100);
	in = SPI0_byte(out >> 8);
	in = SPI0_byte(out) | in << 8;
	SPI0_select(0);
	return in;
}
*/

void MAX3100_init(struct MAX3100 * const device, const enum E_MAX3100_CONFIGURATION conf, const long baud)
{
	device->word(CONFIGURE | (conf & ~(CONFIGURE | BAUD)) | MAX3100_baud(baud));
	RING_clear(&device->RX);
	RING_clear(&device->TX);
}

#define MAX3100_RX_PEND ((device->word(CONFIGURATION) & RX_PENDING) != 0)
#define MAX3100_TX_PEND ((device->word(CONFIGURATION) & TX_PENDING) == 0)

void MAX3100_send(struct MAX3100 * const device)
{
	while (MAX3100_RX_PEND)	// read pending
		RING_put(&device->RX) = device->word(READ);
	while (device->TX.tail != device->TX.head)
	{	// for all sendable bytes
		while (MAX3100_RX_PEND)	// read pending
			RING_put(&device->RX) = device->word(READ);
		while (MAX3100_TX_PEND) // write pending
			NOP();
		device->word(SEND | RING_get(&device->TX));
	}
	while (MAX3100_RX_PEND)	// read pending
		RING_put(&device->RX) = device->word(READ);
}
