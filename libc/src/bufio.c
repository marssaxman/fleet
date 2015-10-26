#include <stdio.h>
#include "internal/stream.h"
#include <sys/socket.h>
#include <sys/stdio.h>
#include <errno.h>

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
	return 0;
}

int _read(FILE *stream, void *dest, size_t bytes)
{
	int ret = read(stream->id, dest, bytes);
	return check_size(ret, bytes, stream);
}

size_t _write(FILE *stream, const void *src, size_t bytes)
{
	int ret = write(stream->id, src, bytes);
	return check_size(ret, bytes, stream);
}

