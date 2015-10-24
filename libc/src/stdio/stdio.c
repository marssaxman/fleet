#include <stdio.h>
#include "internal/stream.h"
#include <sys/syscall.h>
#include <sys/stdio.h>

struct _stream _stdin;
FILE *stdin = &_stdin;
struct _stream _stdout;
FILE *stdout = &_stdout;
struct _stream _stderr;
FILE *stderr = &_stderr;

int _read(FILE *stream, void *dest, size_t bytes)
{
	int n = read(stream->id, dest, bytes);
	if (n < bytes) {
		stream->state |= FILE_ERR;
		if (n <= 0) {
			stream->state |= FILE_EOF;
		}
	}
	return n;
}

int _write(FILE *stream, const void *src, size_t bytes)
{
	int n = write(stream->id, src, bytes);
	if (n < bytes) {
		stream->state |= FILE_ERR;
		if (n <= 0) {
			stream->state |= FILE_EOF;
		}
	}
	return n;
}

void _stdio_init()
{
	// load the stream IDs from sys/stdio
	_stdin.id = _stdin_id;
	_stdout.id = _stdout_id;
	_stderr.id = _stderr_id;
}
