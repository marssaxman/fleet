// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <stddef.h>

struct io_queue {
	struct io_queue **back;
	struct io_queue *next;
};

struct io_buffer {
	void *buffer;
	size_t length;
};

struct io_transfer {
	struct io_buffer buffer;
	struct io_queue queue;
	void (*completion)(struct io_transfer*);
};

#endif //_SYS_IO_H

