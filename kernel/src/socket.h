// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef SOCKET_H
#define SOCKET_H

#include <fleet/io.h>

struct socket_methods {
	int (*open)(void *data);
	int (*read)(void *data, struct stream_transfer*);
	int (*write)(void *data, struct stream_transfer*);
	int (*close)(void *data);
};

#define OPEN_MAX 64

void _socket_init();
int _socket_alloc(struct socket_methods*, void *data);
void _socket_free(int socket);

#endif //SOCKET_H

