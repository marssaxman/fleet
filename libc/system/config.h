#ifndef _LIBC_CONFIG_H
#define _LIBC_CONFIG_H

#include "stream.h"

struct _libc_config
{
	struct _libc_stream *stdin;
	struct _libc_stream *stdout;
	struct _libc_stream *stderr;
	const char *args;
};

#endif //_LIBC_CONFIG_H

