#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

static int bufread(FILE *stream, char *dest, size_t bytes)
{
	size_t total = 0;
	char *buf_end = stream->buf_addr + stream->buf_size;
	if (stream->buf_count > 0) {
		// There is data in this input buffer. Copy as much of it into the
		// destination buffer as we can fit.
		size_t space = bytes - total;
		size_t copy = (space > stream->buf_count)? stream->buf_count: space;
		memcpy(dest, buf_end - stream->buf_count, copy);
		stream->buf_count -= copy;
		total += copy;
		dest += copy;
	}
	size_t remaining = bytes - total;
	if (remaining >= stream->buf_size) {
		// We want to read more data than the stream buffer will hold. Read
		// directly from the socket into the destination buffer instead.
		size_t copy = remaining - (remaining % stream->buf_size);
		int ret = read(stream->id, dest, copy);
		if (ret < 0) return ret;
		total += ret;
		dest += ret;
		if (ret < copy) return total;
		remaining = bytes - total;
	}
	if (remaining > 0) {
		// The buffer is empty, but we still need more data. Refill the stream
		// buffer, then copy as much of it into the destination buffer as it
		// can still hold.
		int ret = read(stream->id, stream->buf_addr, stream->buf_size);
		if (ret < 0) return ret;
		stream->buf_count = ret;
		if (ret > 0 && ret < stream->buf_size) {
			memmove(buf_end - ret, stream->buf_addr, ret);
		}
		memcpy(dest, buf_end - stream->buf_count, remaining);
		stream->buf_count -= remaining;
	}
	return total;
}

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
	int ret = 0;
	if (stream->buf_size > 0) {
		ret = bufread(stream, dest, bytes);
	} else {
		ret = read(stream->id, dest, bytes);
	}
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


