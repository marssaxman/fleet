#include "internal/stream.h"
#include <sys/socket.h>
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
	stream->state &= ~STREAM_READ;
	return 0;
}

int fflush(FILE *stream)
{
	// If stream is NULL, we are supposed to flush *all* streams. Yoiks.
	// Guess we'll have to implement some kind of list of file structs...?
	if (!stream) return EOF;
	if (stream->state & STREAM_WRITE) return flushwrite(stream);
	if (stream->state & STREAM_READ) return flushread(stream);
	stream->buf_count = 0;
	return 0;
}


