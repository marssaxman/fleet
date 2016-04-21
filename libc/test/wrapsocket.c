// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "socket.h"
#include "errno.h"

// Replacement implementations of kernel IO functions, since we don't want
// our libc linking against the similarly-named system call wrappers provided
// by the host's version of libc.
// We will allow the tests to open a single socket at a time. It will always
// have id 1.

static struct iops *socket_ops;
static void *socket_ref;

int libc_test_open(void *ref, struct iops *ops)
{
	if (socket_ops != 0) return -1;
	socket_ref = ref;
	socket_ops = ops;
	return 1;
}

int libc_test_write(int socket, const void *src, unsigned bytes)
{
	if (socket != 1 || socket_ops == 0) return -EBADF;
	if (socket_ops->write == 0) return -EPERM;
	return socket_ops->write(socket_ref, src, bytes);
}

int libc_test_read(int socket, void *dest, unsigned bytes)
{
	if (socket != 1 || socket_ops == 0) return -EBADF;
	if (socket_ops->read == 0) return -EPERM;
	return socket_ops->read(socket_ref, dest, bytes);
}

int libc_test_close(int socket)
{
	if (socket != 1 || socket_ops == 0) return -EBADF;
	if (socket_ops->close) socket_ops->close(socket_ref);
	socket_ops = 0;
	return 0;
}


