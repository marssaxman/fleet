#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

static int bufwrite(FILE *stream, const char *src, size_t bytes)
{
	// If the source data will fit in the stream buffer without filling it,
	// the write operation reduces to a copy.
	size_t bufavail = stream->buf_end - stream->buf_pos;
	if (bufavail > bytes) {
		memcpy(stream->buf_pos, src, bytes);
		stream->buf_pos += bytes;
		return bytes;
	}
	// The data we want to write is no smaller than the buffer space. If there
	// is data in the buffer already, finish filling it and flush it.
	size_t total = 0;
	if (stream->buf_pos > stream->buf_addr) {
		memcpy(stream->buf_pos, src, bufavail);
		total += bufavail;
		stream->buf_pos = stream->buf_end;
		if (fflush(stream)) return EOF;
		src += bufavail;
		bytes -= bufavail;
	}
	// If the source buffer is larger than the stream buffer, skip the copy
	// and write directly from the source buffer, up to an even multiple of
	// the stream buffer size.
	if (bytes >= stream->buf_size) {
		size_t copy = (bytes / stream->buf_size) * stream->buf_size;
		int ret = write(stream->id, src, copy);
		if (ret >= 0) total += ret;
		if (ret < copy) return total;
		bytes -= copy;
		src += copy;
	}
	// Copy the remaining bytes into the stream buffer.
	if (bytes > 0) {
		memcpy(stream->buf_pos, src, bytes);
		stream->buf_pos += bytes;
		total += bytes;
	}
	return total;
}

size_t fwrite(const void *src, size_t size, size_t count, FILE *stream)
{
	size_t bytes = size*count;
	if (0 == bytes) return 0;
	int ret = 0;
	if (stream->buf_addr) {
		ret = bufwrite(stream, src, bytes);
	} else {
		// No buffer. Write the source buffer to the socket directly.
		ret = write(stream->id, src, bytes);
	}
	if (ret < bytes) {
		errno = -ret;
		stream->state |= STREAM_ERR;
		return 0;
	}
	return ret / size;
}


