#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#include <stdio.h>

#define STREAM_EOF 1 // C standard feof() indicator
#define STREAM_ERR 2 // C standard ferror() indicator
#define STREAM_ALLOC 4 // we own the buffer and must free() it on close
#define STREAM_LINESYNC 8 // must flush buffer after each newline

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
