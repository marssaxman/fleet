#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

static char *bufcpy(char *dest, FILE *stream, size_t bytes)
{
	char *buf_pos = &stream->buf_addr[stream->buf_size - stream->buf_count];
	memcpy(dest, buf_pos, bytes);
	stream->buf_count -= bytes;
	return &dest[bytes];
}

static int bufread(FILE *stream, void *dest, size_t bytes)
{
	// If there is enough data in the buffer to satisfy this request, perform
	// a simple copy and return.
	if (stream->buf_count >= bytes) {
		bufcpy(dest, stream, bytes);
		return bytes;
	}
	// We want to read more data than we have in the buffer. That's fine, but
	// we should use up whatever we have before reading more from the socket.
	size_t remaining = bytes;
	if (stream->buf_count > 0) {
		dest = bufcpy(dest, stream, stream->buf_count);
		remaining -= stream->buf_count;
	}
	// If the amount we want to read is greater than the buffer size, read
	// directly into the destination buffer instead of copying through the
	// stream. We'll read in even units of the buffer size, though, and read
	// a full buffer's worth at the end.
	if (remaining >= stream->buf_size) {
		size_t copy = remaining - (remaining % stream->buf_size);
		int ret = read(stream->id, dest, copy);
		if (ret >= 0) {
			remaining -= ret;
			dest += ret;
		}
		if (ret < copy) return bytes - remaining;
	}
	// If there's more data to read, read a full buffer's worth, then copy out
	// as much as we need to complete the current request.
	if (remaining > 0) {
		int ret = read(stream->id, stream->buf_addr, stream->buf_size);
		if (ret > 0 && ret < stream->buf_size) {
			char *buf_pos = &stream->buf_addr[stream->buf_size - ret];
			memcpy(buf_pos, stream->buf_addr, ret);
		}
		if (ret > 0) {
			stream->buf_count = ret;
			remaining -= ret;
		}
	}
	return bytes - remaining;
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


