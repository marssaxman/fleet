
#include "uart.h"
#include "cpu.h"
#include "irq.h"
#include <stdint.h>
#include <sys/errno.h>
#include "socket.h"
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

struct buf
{
	char *base;
	char *pos;
	char *limit;
};

struct uart {
	unsigned port;
	unsigned irq;
	int streamid;
	// we are transferring data if transmit != 0
	struct transfer *transmit;
	struct buf xmit;
	// we are receiving data if receive != 0
	struct transfer *receive;
	struct buf rcev;
};

struct uart COM1 = { .port = 0x3F8, .irq = 4 };
struct uart COM2 = { .port = 0x2F8, .irq = 3 };
struct uart COM3 = { .port = 0x3E8, .irq = 4 };
struct uart COM4 = { .port = 0x2E8, .irq = 3 };

static int uart_transmit(void*, struct transfer*);
static int uart_receive(void*, struct transfer*);
static int uart_sync(void*);
static void uart_close(void*);

static struct iops uart_ops = {
	.transmit = uart_transmit,
	.receive = uart_receive,
	.sync = uart_sync,
	.close = uart_close,
};

static void begin(struct transfer *xfer, struct buf *buf)
{
	if (xfer) {
		buf->pos = buf->base = xfer->buffer;
		buf->limit = buf->base + xfer->length;
	}
}

static void complete(struct transfer **xfer_field, struct buf *buf)
{
	struct transfer *xfer = *xfer_field;
	xfer->completion(xfer, buf->limit - buf->base);
	*xfer_field = 0;
}

static void disable_interrupts(struct uart *uart)
{
	unsigned ier = uart->port + IER;
	_outb(ier, 0);
}

static void enable_interrupts(struct uart *uart)
{
	unsigned ier = uart->port + IER;
	unsigned flags = _inb(ier);
	flags = uart->transmit? (flags|IER_THRE): (flags & ~IER_THRE);
	flags = uart->receive? (flags|IER_RX_DATA): (flags & ~IER_RX_DATA);
	_outb(ier, flags);
}

static void xmitloop(struct uart *uart)
{
	unsigned lsr = uart->port + LSR;
	unsigned thr = uart->port + THR;
	unsigned ier = uart->port + IER;
	// Keep working as long as we have data to send and a port ready to
	// transmit it.
	while (uart->transmit && (_inb(lsr) & LSR_TX_READY)) {
		if (uart->xmit.pos < uart->xmit.limit) {
			_outb(thr, *(uart->xmit.pos)++);
		} else {
			// We ran out of data. Call the completion proc, which may take
			// this as an opportunity to queue up some more data.
			complete(&uart->transmit, &uart->xmit);
		}
	}
}

static void rcevloop(struct uart *uart)
{
	unsigned lsr = uart->port + LSR;
	unsigned rbr = uart->port + RBR;
	unsigned ier = uart->port + IER;
	// Keep working as long as we have a buffer ready to receive and a port
	// with data to put in it.
	while (uart->receive && (_inb(lsr) & LSR_RX_READY)) {
		if (uart->rcev.pos < uart->rcev.limit) {
			*(uart->rcev.pos)++ = _inb(rbr);
		} else {
			// We ran out of buffer. Call its completion proc, which will
			// either return a new transfer object to begin receiving from, or
			// will return NULL and end the loop.
			complete(&uart->receive, &uart->rcev);
		}
	}
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
		case IIR_THRE: {
			// Transmit holding register empty: time to send more data.
			xmitloop(uart);
		} break;
		case IIR_RX_DATA: {
			// Transmit receive buffer non-empty: time to read some bytes.
			rcevloop(uart);
		} break;
		case IIR_RX_STATUS: {
			// nothing we can usefully do just yet, but we need to clear
			// the condition, so we'll read from the status register
        	_inb(uart->port + LSR);
		} break;
	}
	enable_interrupts(uart);
}

int _uart_open(struct uart *uart)
{
	if (uart->streamid) return EISCONN;
	// Switch DLAB on and set the speed to 115200.
	_outb(uart->port + LCR, LCR_DLAB);
	_outb(uart->port + DLL, 0x01);
	_outb(uart->port + DLH, 0x00);
	// Switch DLAB off and configure 8N1 mode.
	_outb(uart->port + LCR, LCR_WORD_8);
	// Enable FIFO mode and clear buffers.
	_outb(uart->port + FCR, FIFO_ENABLE|FIFO_CLEAR_ALL|FIFO_TRIGGER_14);
	// Install our proc as the handler for this port's IRQ.
	_irq_listen(uart->irq, uart, &uart_irq);
	// Create and return a stream ID so system calls can refer to this.
	uart->streamid = _open(uart, &uart_ops);
}

static int uart_transmit(void *ref, struct transfer *xfer)
{
	struct uart *uart = (struct uart*)ref;
	disable_interrupts(uart);
	int err = 0;
	if (0 == uart->transmit) {
		uart->transmit = xfer;
		begin(xfer, &uart->xmit);
		xmitloop(uart);
	} else {
		err = EBUSY;
	}
	enable_interrupts(uart);
	return err;
}

static int uart_receive(void *ref, struct transfer *xfer)
{
	struct uart *uart = (struct uart*)ref;
	disable_interrupts(uart);
	int err = 0;
	if (0 == uart->receive) {
		uart->receive = xfer;
		begin(xfer, &uart->rcev);
		rcevloop(uart);
	} else {
		err = EBUSY;
	}
	enable_interrupts(uart);
	return err;
}

static int uart_sync(void *ref)
{
	struct uart *uart = (struct uart*)ref;
	disable_interrupts(uart);
	if (uart->receive) complete(&uart->receive, &uart->rcev);
	if (uart->transmit) complete(&uart->transmit, &uart->xmit);
	enable_interrupts(uart);
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



