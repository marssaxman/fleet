#ifndef LIBC_TEST_MEMSOCKET_H
#define LIBC_TEST_MEMSOCKET_H

struct memsocket
{
	void *buf_addr;
	unsigned buf_size;
	unsigned data_len;
};

int open_memsocket(struct memsocket*);

#endif //LIBC_TEST_MEMSOCKET_H
