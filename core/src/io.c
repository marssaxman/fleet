#include <sys/io.h>

void io_send(io_socket_t *dest, io_buffer_t *src)
{
	dest->mechanism->send(dest, src);
}

void io_receive(io_socket_t *src, io_buffer_t *dest)
{
	src->mechanism->receive(src, dest);
}

void io_close(io_socket_t *target)
{
	target->mechanism->close(target);
}

void io_connect(io_port_t *port, io_socket_t *socket)
{
	port->mechanism->connect(port, socket);
}

void io_listen(io_port_t *port, io_socket_t *socket)
{
	port->mechanism->listen(port, socket);
}

void io_bind(io_transport_t *transport, io_port_t *port, const char *address)
{
	transport->mechanism->bind(transport, port, address);
}
