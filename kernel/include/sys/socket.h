#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

// A socket is a communication endpoint.
typedef int socket_t;

// A transfer record describes a transfer of data through a socket.
struct transfer
{
	struct transfer *(*completion)(struct transfer*, size_t bytes);
	void *buffer;
	size_t length;
};

// Begin reading data from the transfer buffer and sending it through the
// socket. A transmission ends when the communication channel blocks, the
// socket is synced, or the buffer is exhausted; the completion proc will be
// invoked, receiving the number of bytes that were sent.
int transmit(socket_t, struct transfer*);
// Returns an error code or 0 for success.

// Begin receiving data from the socket and writing it into the buffer. The
// reception ends when there are no more bytes to read, the socket is synced,
// or the buffer becomes full. The completion proc will be invoked, receiving
// the number of bytes that were transmitted.
int receive(socket_t, struct transfer*);
// Returns an error code or 0 for success.

// Flush buffers and synchronize state. If there are transfers active, they
// will be stopped and their notification procedures invoked.
int sync(socket_t);
// Returns an error code or 0 for success.

// Sync the socket state, then close it. The socket ID becomes invalid.
int close(socket_t);
// Returns an error code or 0 for success.

#endif //_SYS_SOCKET_H

