#ifndef _LIBC_SYSTEM_H
#define _LIBC_SYSTEM_H

#include "stream.h"

struct _libc_config
{
	struct _libc_stream *stdin;
	struct _libc_stream *stdout;
	struct _libc_stream *stderr;
	const char *args;
};

void _libc_init(const struct _libc_config*);

#endif //_LIBC_SYSTEM_H

