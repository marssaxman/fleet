
#include "uart.h"
#include "cpu.h"
#include "irq.h"
#include <stdint.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include "panic.h"

// Implementation of the primitive PC serial transport.

// more information about serial ports than you could ever want:
// http://retired.beyondlogic.org/serial/serial.htm

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

// The modem status register describes communication signalling states.
#define MSR_DCD 0x80	// carrier detect
#define MSR_RI 0x40 // ring indicator
#define MSR_DSR 0x20	// data set ready
#define MSR_CTS 0x10	// clear to send
#define MSR_X_DCD 0x08	// change in DCD
#define MSR_X_RI 0x04	// low RI went high
#define MSR_X_DSR 0x02 // change in DSR
#define MSR_X_CTS 0x01 // change in CTS

struct uart {
	unsigned port;
	unsigned irq;
	int streamid;
};

struct uart COM1 = { .port = 0x3F8, .irq = 4 };
struct uart COM2 = { .port = 0x2F8, .irq = 3 };
struct uart COM3 = { .port = 0x3E8, .irq = 4 };
struct uart COM4 = { .port = 0x2E8, .irq = 3 };

static int uart_read(void *ref, void *buf, unsigned bytes);
static int uart_write(void *ref, const void *buf, unsigned bytes);
static void uart_close(void *ref);

static struct iops uart_ops = {
	.read = uart_read,
	.write = uart_write,
	.close = uart_close,
};

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
		case IIR_THRE: /* TX ready */ break;
		case IIR_RX_DATA: /* RX ready */ break;
		case IIR_RX_STATUS: {
			// nothing we can usefully do just yet, but we need to clear
			// the condition, so we'll read from the status register
        	_inb(uart->port + LSR);
		} break;
	}
}

int _uart_open(struct uart *uart)
{
	if (uart->streamid) return -EISCONN;
	// Switch DLAB on and set the speed to 115200.
	_outb(uart->port + LCR, LCR_DLAB);
	_outb(uart->port + DLL, 0x01);
	_outb(uart->port + DLH, 0x00);
	// Switch DLAB off and configure 8N1 mode.
	_outb(uart->port + LCR, LCR_WORD_8);
	// Enable FIFO mode and clear buffers.
	_outb(uart->port + FCR, FIFO_ENABLE|FIFO_CLEAR_ALL|FIFO_TRIGGER_14);
	// Add ourselves to the notify queue for the port's IRQ.
	_irq_listen(uart->irq, uart, &uart_irq);
	// Enable interrupts so we don't have to waste time polling.
	// We want to know when data is ready to send or to receive.
	_outb(uart->port + IER, IER_RX_DATA|IER_THRE);
	// Create and return a stream ID so system calls can refer to this.
	uart->streamid = open(uart, &uart_ops);
}

static int uart_write(void *ref, const void *buf, unsigned bytes)
{
	struct uart *uart = (struct uart*)ref;
	const char *p = buf;
	const char *end = buf + bytes;
	unsigned lsr_addr = uart->port + LSR;
	unsigned thr_addr = uart->port + THR;
	// Write into the port until our buffer is empty or its buffer is full.
	while (_inb(lsr_addr) & LSR_TX_READY && p < end) {
		// Add this byte to the FIFO.
		_outb(thr_addr, *p++);
	}
	// Return the number of bytes we read.
	return p - (char*)buf;
}

static int uart_read(void *ref, void *buf, unsigned capacity)
{
	struct uart *uart = (struct uart*)ref;
	char *p = buf;
	char *end = buf + capacity;
	unsigned lsr_addr = uart->port + LSR;
	unsigned rbr_addr = uart->port + RBR;
	// Read bytes from this port until we empty it or run out of buffer.
	while (_inb(lsr_addr) & LSR_RX_READY && p < end) {
		*p++ = _inb(rbr_addr);
	}
	return p - (char*)buf;
}

static void uart_close(void *ref)
{
	struct uart *uart = (struct uart*)ref;
	assert(uart->streamid != 0);
	uart->streamid = 0;
	// Disable interrupts.
	_outb(uart->port + IER, 0);
	// Detach from the ISR.
	_irq_ignore(uart->irq);
}



