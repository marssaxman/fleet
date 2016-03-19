// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"
#include "syscalls.h"
#include <errno.h>

static int flushwrite(FILE *stream)
{
	// For output streams, and update streams where the last operation was
	// output, flush must write any unwritten data from the buffer.
	int ret = 0;
	if (stream->buf_count > 0) {
		ret = write(stream->id, stream->buf_addr, stream->buf_count);
		if (ret < stream->buf_count) {
			errno = (ret < 0)? -ret: -1;
			stream->state |= STREAM_ERR;
			ret = EOF;
		}
		stream->buf_count = 0;
	}
	stream->state &= ~STREAM_WRITE;
	return ret;
}

static int flushread(FILE *stream)
{
	// For input streams and update streams where the last operation was
	// input, the behavior is undefined. We will simply clear the state.
	stream->state &= ~(STREAM_READ|STREAM_UNGET);
	return 0;
}

int fflush(FILE *stream)
{
	// If stream is NULL, we are supposed to flush all output streams.
	// Otherwise, we flush the specified stream only.
	if (stream) {
		if (stream->state & STREAM_WRITE) return flushwrite(stream);
		if (stream->state & STREAM_READ) return flushread(stream);
		stream->buf_count = 0;
	} else for (stream = _stream_list; stream; stream = stream->next) {
		if (stream->state & STREAM_WRITE) {
			int ret = flushwrite(stream);
			if (ret != 0) return ret;
		}
	}
	return 0;
}


