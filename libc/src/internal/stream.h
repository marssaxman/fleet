#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#define FILE_EOF 1
#define FILE_ERR 2

struct _stream
{
	int id;
	unsigned state;
};

#endif //_LIBC_INTERNAL_STREAM_H
