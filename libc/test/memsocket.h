// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef LIBC_TEST_MEMSOCKET_H
#define LIBC_TEST_MEMSOCKET_H

struct memsocket
{
	void *buf_addr;
	unsigned buf_size;
	unsigned data_pos;
	unsigned data_len;
};

int open_memsocket(void *addr, unsigned size, struct memsocket*);

#endif //LIBC_TEST_MEMSOCKET_H
