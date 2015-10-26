#include <stdio.h>
#include "internal/stream.h"
#include <sys/socket.h>
#include <sys/stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

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
	int ret = sync(stream->id);
	stream->state &= ~(STREAM_WRITE|STREAM_READ);
	return check_err(ret, stream);
}

void _buffer(FILE *stream, void *buf, size_t bytes)
{
}

struct buffer_xfer
{
	struct transfer xfer;
	volatile bool done;
	volatile size_t bytes;
};

static struct transfer *complete(struct transfer *xfer, size_t bytes)
{
	((struct buffer_xfer*)xfer)->bytes = bytes;
	((struct buffer_xfer*)xfer)->done = true;
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
	struct buffer_xfer bfx = {{complete, dest, bytes}, 0, 0};
	int ret = receive(stream->id, &bfx.xfer);
	if (ret) return -ret;
	while (!bfx.done) { /* sleep */ }
	return bfx.bytes;
}

static struct transfer *read_complete(struct transfer *xfer, size_t bytes)
{
}

size_t _write(FILE *stream, const void *src, size_t bytes)
{
	// Check direction: must sync or setpos before writing after reading.
	if (stream->state & STREAM_READ) {
		stream->state |= STREAM_ERR;
		return 0;
	}
	stream->state |= STREAM_WRITE;
	struct buffer_xfer bfx = {{complete, (void*)src, bytes}, 0, 0};
	int ret = transmit(stream->id, &bfx.xfer);
	if (ret) return -ret;
	while (!bfx.done) { /* sleep */ }
	return bfx.bytes;
}

