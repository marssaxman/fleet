#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#include <stdio.h>

#define STREAM_EOF 0x01 // C standard feof() indicator
#define STREAM_ERR 0x02 // C standard ferror() indicator
#define STREAM_ALLOC 0x04 // we own the buffer and must free() it on close
#define STREAM_LINESYNC 0x08 // must flush buffer after each newline
#define STREAM_READ 0x0F // currently in read mode
#define STREAM_WRITE 0x10 // currently in write mode

struct _stream
{
	unsigned state;
	// stream id used in kernel calls
	int id;
	// buffer location & size, if present
	char *buf_addr;
	size_t buf_size;
	// range of buffer contents currently in use
	char *buf_pos;
	char *buf_end;
};

#endif //_LIBC_INTERNAL_STREAM_H
