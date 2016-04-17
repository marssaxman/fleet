// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef SOCKET_H
#define SOCKET_H

#include <sys/io.h>

struct socket {
	struct operations *op;
	struct io_queue tx;
	struct io_queue rx;
};

struct operations {
	void (*transmit)(struct socket*, struct io_transfer*);
	void (*receive)(struct socket*, struct io_transfer*);
};

#endif //SOCKET_H


