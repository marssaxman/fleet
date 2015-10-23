#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#include "system/stream.h"

struct _stream
{
	struct _libc_stream port;
};

#endif //_LIBC_INTERNAL_STREAM_H
