#ifndef _SYS_IO_H
#define _SYS_IO_H

typedef struct io_buffer io_buffer_t;
typedef struct io_socket io_socket_t;
typedef struct io_port io_port_t;
typedef struct io_transport io_transport_t;

// A buffer is a region of memory used to transfer data to or from a socket.
struct io_buffer {
	void *address;
	unsigned length;
};

// A socket is a communication channel.
struct io_socket {
	struct io_socket_actions {
		void (*send)(io_socket_t*, io_buffer_t*);
		void (*receive)(io_socket_t*, io_buffer_t*);
		void (*close)(io_socket_t*);
	} *mechanism;
	void *reference;
	io_port_t *port;
};
void io_send(io_socket_t*, io_buffer_t*);
void io_receive(io_socket_t*, io_buffer_t*);
void io_close(io_socket_t*);

// A port is an endpoint for a socket.
struct io_port {
	struct io_port_actions {
		void (*connect)(io_port_t*, io_socket_t*);
		void (*listen)(io_port_t*, io_socket_t*);
	} *mechanism;
	void *reference;
	io_transport_t *transport;
};
void io_connect(io_port_t*, io_socket_t*);
void io_listen(io_port_t*, io_socket_t*);

// A transport associates a network address with a port.
struct io_transport {
	struct io_transport_actions {
		void (*bind)(io_transport_t*, io_port_t*, const char *address);
	} *mechanism;
	void *reference;
};
void io_bind(io_transport_t*, io_port_t*, const char *address);

#endif //_SYS_IO_H
