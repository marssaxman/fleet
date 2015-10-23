#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#include <stream.h>

#define FILE_EOF 1
#define FILE_ERR 2

struct _stream
{
	struct _libc_stream port;
	unsigned state;
};

#endif //_LIBC_INTERNAL_STREAM_H
