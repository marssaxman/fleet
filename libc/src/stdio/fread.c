#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>

size_t fread(void *dest, size_t size, size_t count, FILE *stream)
{
	size_t bytes = size*count;
	if (0 == bytes) return 0;
	// Must not read from a stream used for writing without flushing first.
	if (stream->state & STREAM_WRITE) {
		errno = -1; // need a better error code here
		stream->state |= STREAM_ERR;
		return 0;
	}
	stream->state |= STREAM_READ;
	int ret = read(stream->id, dest, bytes);
	if (ret < 0) {
		errno = -ret;
		stream->state |= STREAM_ERR;
		return 0;
	}
	if (ret < bytes) {
		stream->state |= STREAM_EOF;
	}
	return ret / size;
}


