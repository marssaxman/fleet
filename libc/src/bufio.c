#include <stdio.h>
#include "internal/stream.h"
#include <sys/syscall.h>
#include <sys/stdio.h>
#include <errno.h>
#include <string.h>

// Check the result of a system call: if it was an error code, set errno,
// set the stream's error flag, and return the C error code instead (EOF).
// If the call succeeded, return its result unchanged.
static int check_err(int ret, FILE *stream)
{
	if (ret < 0) {
		errno = -ret;
		stream->state |= STREAM_ERR;
		ret = EOF;
	}
	return ret;
}

// Check the result of a system call: if it was an error, set errno and the
// stream's error flag, then return 0. If the call did not return an error,
// but the size was zero, set the EOF indicator. In any case, the result shall
// be the number of bytes and not an error code.
static size_t check_size(int n, size_t bytes, FILE *stream)
{
	n = check_err(n, stream);
	if (n < bytes) {
		stream->state |= STREAM_ERR;
		if (n == 0) {
			stream->state |= STREAM_EOF;
		}
		if (n < 0) {
			n = 0;
		}
	}
	return n;
}

int _close(FILE *stream)
{
	int ret = close(stream->id);
	return check_err(ret, stream);
}

int _flush(FILE *stream)
{
	int ret = flush(stream->id);
	return check_err(ret, stream);
}

void _buffer(FILE *stream, void *buf, size_t bytes)
{
	stream->buf = buf;
	stream->buflen = bytes;
	stream->pos = buf;
	stream->limit = buf;
}

int _read(FILE *stream, void *dest, size_t bytes)
{
	// Once data has been written to a stream, it is illegal to read from it
	// until you flush it or set position.
	if (stream->state & STREAM_WRITE) {
		stream->state |= STREAM_ERR;
		return 0;
	}
	stream->state |= STREAM_READ;
	// If this is an unbuffered stream, go make the syscall immediately.
	if (0 == stream->buf) {
		int ret = read(stream->id, dest, bytes);
		return check_size(ret, bytes, stream);
	}
	size_t remaining = bytes;
	while (remaining > 0) {
		size_t avail = stream->tail - stream->head;
		size_t copy = (avail <= remaining)? avail: remaining;
		dest = memcpy(dest, stream->head, copy) + copy;
		stream->head += copy;
		remaining -= copy;
		if (stream->head == stream->tail) {
			// The buffer's empty; time to refill it.
			stream->head = stream->tail = stream->buf;
			int ret = read(stream->id, stream->buf, stream->buflen);
			if (ret < 0) return check_err(ret, stream);
			if (ret == 0) break;
			stream->tail += ret;
		}
	}
	// Partial success: return the quantity read.
	return check_size(bytes - remaining, bytes, stream);
}

size_t _write(FILE *stream, const void *src, size_t bytes)
{
	// Check direction: must sync or setpos before writing after reading.
	if (stream->state & STREAM_READ) {
		stream->state |= STREAM_ERR;
		return 0;
	}
	// If there's no buffer, pass the write through to the kernel.
	if (0 == stream->buflen) {
		int ret = write(stream->id, src, bytes);
		return check_size(ret, bytes, stream);
	}
	size_t remaining = bytes;
	while (remaining > 0) {
		// Copy data into the end of the buffer.
		size_t avail = stream->buflen - (stream->tail - stream->buf);
		size_t copy = (avail <= remaining)? avail: remaining;
		
		// If the buffer is full, flush it.
		
	}

}

