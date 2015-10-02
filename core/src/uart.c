#include <sys/uart.h>
#include <stdint.h>
#include "cpu.h"

// Implementation of the primitive PC serial transport.

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
	_outb(address + 3, 0x80); // enable DLAB
	_outb(address + 0, 0x01); // set divisor to 1 (115200baud)
	_outb(address + 1, 0x00); // hi byte of divisor
	_outb(address + 3, 0x03); // 8N1 
	_outb(address + 2, 0xC7); // enable FIFO
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

