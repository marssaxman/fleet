#include <stdio.h>
#include "internal/stream.h"
#include <sys/syscall.h>

struct _stream _stdin = { .id = STDIN_FILENO };
FILE *stdin = &_stdin;

struct _stream _stdout = { .id = STDOUT_FILENO };
FILE *stdout = &_stdout;

struct _stream _stderr = { .id = STDERR_FILENO };
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

