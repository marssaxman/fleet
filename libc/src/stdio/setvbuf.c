// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"
#include <stdlib.h>

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	if (!stream) return -1;
	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF) return -1;
	// Functionally speaking, size = 0 also means _IONBF regardless of mode.
	if (mode == _IONBF) {
		// The standard says we will ignore the buffer and size parameters
		// when configuring a stream for no buffer.
		buffer = NULL;
		size = 0;
	}
	// If this stream previously had a heap-allocated buffer, release it now.
	if (stream->state & STREAM_ALLOC) {
		free(stream->buf_addr);
		stream->state &= ~STREAM_ALLOC;
	}
	// If buffer is NULL and size > 0, we are supposed to malloc a buffer.
	if (buffer == NULL && size > 0) {
		buffer = malloc(size);
		stream->state |= STREAM_ALLOC;
	}
	// Configure the buffer.
	stream->buf_addr = buffer;
	stream->buf_size = size;
	stream->buf_count = 0;
	// Set the line sync flag, which is an alternate limit on buffer size.
	if (mode == _IOLBF) {
		stream->state |= STREAM_LINESYNC;
	} else {
		stream->state &= ~STREAM_LINESYNC;
	}
}

