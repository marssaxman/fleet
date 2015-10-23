#include <stdio.h>
#include "internal/stream.h"

// system/libc.h contains extern _libc_stream declarations for these names,
// but we define them as _stream since the _libc_stream is the first member
// in our FILE struct.

struct _stream _stdin;
FILE *stdin = &_stdin;

struct _stream _stdout;
FILE *stdout = &_stdout;

struct _stream _stderr;
FILE *stderr = &_stderr;

int _read(FILE *stream, void *dest, size_t bytes)
{
	struct _libc_stream *port = &stream->port;
	size_t n = port->read? port->read(port->ref, dest, bytes): 0;
	if (n < bytes) {
		stream->state |= FILE_ERR;
		if (0 == n) {
			stream->state |= FILE_EOF;
		}
	}
	return n;
}

int _write(FILE *stream, const void *src, size_t bytes)
{
	struct _libc_stream *port = &stream->port;
	size_t n = port->write? port->write(port->ref, src, bytes): 0;
	if (n < bytes) {
		stream->state |= FILE_ERR;
		if (0 == n) {
			stream->state |= FILE_EOF;
		}
	}
	return n;
}

