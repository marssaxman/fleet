#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

struct iops
{
	void (*close)(void *ref);
	int (*sync)(void *ref);
	int (*transmit)(void *ref, struct transfer*);
	int (*receive)(void *ref, struct transfer*);
};

socket_t _open(void *ref, const struct iops*);

#endif //SOCKET_H

