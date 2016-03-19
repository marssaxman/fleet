// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"
#include "syscalls.h"
#include <stdlib.h>

int fclose(FILE *stream)
{
	int flushret = fflush(stream);
	int closeret = close(stream->id);
	if (stream->state & STREAM_ALLOC) {
		free(stream->buf_addr);
	}
	_exit_stream(stream);
	return (flushret || closeret)? EOF: 0;
}

