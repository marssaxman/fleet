#include <sys/uart.h>
#include <stdint.h>
#include "cpu.h"

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

static void send(io_socket_t *self, io_buffer_t *buf)
{
	uintptr_t address = (intptr_t)self->port->reference;
	// bad crude blocking implementation. get some interrupts in here
	char *p = (char*)buf->address;
	char *end = p + buf->length;
	while (p < end) {
		// Wait until the port is ready to receive
		while (0 == (_inb(address + 5) & 0x20)) { /*wait*/ }
		// Push another byte into its buffer
		_outb(address, *p++);
	}
}

static void receive(io_socket_t *self, io_buffer_t *buf)
{
}

static void close(io_socket_t *self)
{
}

static struct io_socket_actions socket_mechanism = {
	.send = send,
	.receive = receive,
	.close = close,
};

static void enable(io_port_t *self, io_socket_t *socket)
{
	uintptr_t address = (intptr_t)self->reference;
	// Switch DLAB on and set the speed to 115200.
	_outb(address + LCR, LCR_DLAB);
	_outb(address + DLL, 0x01);
	_outb(address + DLH, 0x00);
	// Switch DLAB off and configure 8N1 mode.
	_outb(address + LCR, LCR_WORD_8);
	// Enable FIFO mode and clear buffers.
	_outb(address + FCR, FIFO_ENABLE|FIFO_CLEAR_ALL|FIFO_TRIGGER_14);
	// Enable interrupts so we don't have to waste time polling.
	// We want to know when data is ready to send or to receive.
	_outb(address + IER, IER_RX_DATA|IER_THRE);
	// Configure the socket and let 'er rip.
	socket->mechanism = &socket_mechanism;
	socket->reference = 0;
	socket->port = self;
}

static void connect(io_port_t *self, io_socket_t *socket)
{
	enable(self, socket);
	// detect/report errors, etc
}

static void listen(io_port_t *self, io_socket_t *socket)
{
	enable(self, socket);
	// how to detect/report failure?
	// what if the port's already in use?
}

static struct io_port_actions port_mechanism = {
	.connect = connect,
	.listen = listen,
};

static void bind(io_transport_t *self, io_port_t *port, const char *name)
{
	// Resolve this address, which should be the number of a COM port.
	int number = *name++ - '1';
	if (number < 0 || number > 3 || *name) {
		// complain, somehow; TODO
		return;
	}
	static intptr_t addrs[4] = {0x3F8, 0x2F8, 0x3E8, 0x2E8};
	port->mechanism = &port_mechanism;
	port->reference = (void*)(addrs[number]);
	port->transport = self;
	// report success, somehow; TODO
}

static struct io_transport_actions transport_mechanism = {
	.bind = bind,
};

io_transport_t uart = {
	.mechanism = &transport_mechanism,
	.reference = 0,
};

