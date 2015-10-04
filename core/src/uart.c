
#include "uart.h"
#include "cpu.h"
#include "irq.h"
#include "events.h"
#include <stdint.h>

// Implementation of the primitive PC serial transport.

// The UART has six register addresses. The meaning depends on the state of
// the DLAB flag and whether you are reading from or writing to the register.
#define RBR 0	// Receive buffer: DLAB=0, input
#define THR 0	// Transmit holding: DLAB=0, output
#define DLL 0	// Divisor latch low: DLAB=1
#define IER 1	// Interrupt enable: DLAB=0
#define DLH 1	// Divisor latch high: DLAB=1
#define IIR 2	// Interrupt ID, input
#define FCR 2	// FIFO control, output
#define LCR 3	// Line control and DLAB switch
#define MCR 4	// Modem control
#define LSR 5	// Line status
#define MSR 6	// Modem status

// There are four events that can provide interrupts.
// OR these flags together to indicate the interrupts you want.
#define IER_RX_DATA 1		// Data ready
#define IER_THRE 2			// Transfer holding register empty
#define IER_RX_STATUS 4 	// PE, FE, overrun, BREAK
#define IER_MODEM_STATUS 8	// CTS, DSR, RI, DCD

// The low three bits of the IIR identify the highest-priority interrupt
// event that has occurred.
#define IIR_MASK 7
#define IIR_NONE 1
#define IIR_MODEM_STATUS 0
#define IIR_THRE 2
#define IIR_RX_DATA 4
#define IIR_RX_STATUS 6

// The FCR configures the FIFO options.
#define FIFO_ENABLE 0x01
#define FIFO_CLEAR_RX 0x02
#define FIFO_CLEAR_TX 0x04
#define FIFO_CLEAR_ALL 0x06
#define FIFO_DMA_MODE 0x08	// switches from mode 0 to mode 1?
#define FIFO_64 0x10	// 64-byte FIFO instead of 16
// Pick one:
#define FIFO_TRIGGER_1 0x00
#define FIFO_TRIGGER_4 0x40
#define FIFO_TRIGGER_8 0x80
#define FIFO_TRIGGER_14 0xC0

// The line control register configures the serial protocol.
#define LCR_DLAB 0x80	// switch divisor registers on or off
#define LCR_BREAK 0x40	// continuously write out spaces
#define LCR_PARITY_MODE 0x08	// use a parity mode
#define LCR_PARITY_STICKY 0x20	// sticky parity
#define LCR_PARITY_EVEN 0x10	// otherwise odd
#define LCR_EXTRA_STOP 0x40	// 1.5/2 stop bits mode, otherwise 1 stop bit
#define LCR_WORD_8 0x03 // use this
#define LCR_WORD_7 0x02 // why would you do this?
#define LCR_WORD_6 0x01 // pain
#define LCR_WORD_5 0x00 // huh?

// The line status register tells us what the port is up to.
#define LSR_RX_READY 0x01
#define LSR_RX_OVERRUN 0x02
#define LSR_PARITY_ERROR 0x04
#define LSR_FRAMING_ERROR 0x08
#define LSR_BREAK 0x10
#define LSR_TX_READY 0x20
#define LSR_TX_STALL 0x40
#define LSR_FIFO_ERROR 0x80


struct uart COM1 = { .port = 0x3F8, .irq = 4 };
struct uart COM2 = { .port = 0x2F8, .irq = 3 };
struct uart COM3 = { .port = 0x3E8, .irq = 4 };
struct uart COM4 = { .port = 0x2E8, .irq = 3 };

static void raise_opt(struct work_item *task)
{
	if (task) raise_event(task);
}

static void uart_irq(void *ref)
{
	struct uart *uart = (struct uart*)ref;
	// See what's up with this port. Why did an interrupt occur?
	uint8_t iir = _inb(uart->port + IIR);
	switch (iir & IIR_MASK) {
		case IIR_NONE: return; // well that was weird
		case IIR_MODEM_STATUS: {
			// we don't really care, but we need to clear the condition,
			// so we'll read from the status register
        	_inb(uart->port + MSR);
		} break;
		case IIR_THRE: raise_opt(uart->events.tx_clear); break;
		case IIR_RX_DATA: raise_opt(uart->events.rx_ready); break;
		case IIR_RX_STATUS: {
			// nothing we can usefully do just yet, but we need to clear
			// the condition, so we'll read from the status register
        	_inb(uart->port + LSR);
		} break;
	}
}

void uart_open(struct uart *uart)
{
	// Configure the listen task we'll attach to the IRQ handler.
	work_item_init(&uart->listen, uart_irq, uart);
	// Switch DLAB on and set the speed to 115200.
	_outb(uart->port + LCR, LCR_DLAB);
	_outb(uart->port + DLL, 0x01);
	_outb(uart->port + DLH, 0x00);
	// Switch DLAB off and configure 8N1 mode.
	_outb(uart->port + LCR, LCR_WORD_8);
	// Enable FIFO mode and clear buffers.
	_outb(uart->port + FCR, FIFO_ENABLE|FIFO_CLEAR_ALL|FIFO_TRIGGER_14);
	// Add ourselves to the notify queue for the port's IRQ.
	irq_listen(uart->irq, &uart->listen);
	// Enable interrupts so we don't have to waste time polling.
	// We want to know when data is ready to send or to receive.
	_outb(uart->port + IER, IER_RX_DATA|IER_THRE);
}

size_t uart_write(struct uart *uart, const char *buf, size_t bytes)
{
	// Write this buffer into the port until its buffer is full.
	const char *p = buf;
	const char *end = buf + bytes;
	while (p < end) {
		// Make sure the UART can receive more data.
		uint8_t lsr = _inb(uart->port + LSR);
		if (0 == (lsr & LSR_TX_READY)) {
			// Wait to send more until the UART signals buffer cleared.
			irq_listen(uart->irq, &uart->listen);
			break;
		}
		// Add this byte to the FIFO.
		_outb(uart->port + THR, *p++);
	}
	// Return the number of bytes we read.
	return p - buf;
}

size_t uart_read(struct uart *uart, char *buf, size_t capacity)
{
	// Read bytes from this port until we empty it or run out of buffer.
	char *p = buf;
	char *end = buf + capacity;
	while (p < end) {
		uint8_t lsr = _inb(uart->port + LSR);
		if (0 == (lsr & LSR_RX_READY)) {
			// Wait until the port signals data availability.
			irq_listen(uart->irq, &uart->listen);
			break;
		}
		*p++ = _inb(uart->port + RBR);
	}
	return p - buf;
}

void uart_close(struct uart *uart)
{
	// Disable interrupts.
	_outb(uart->port + IER, 0);
}


