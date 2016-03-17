#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

struct iops
{
	int (*write)(void *ref, const void *src, unsigned bytes);
	int (*read)(void *ref, void *dest, unsigned bytes);
	void (*close)(void *ref);
};

int open(void *ref, struct iops *ops);
int write(int socket, const void *src, unsigned bytes);
int read(int socket, void *dest, unsigned bytes);
int close(int socket);

#endif //_SYS_SOCKET_H
