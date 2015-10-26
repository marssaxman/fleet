#include "internal/bufio.h"
#include "internal/stream.h"

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	switch (mode) {
		case _IONBUF: buffer = NULL; size = 0; break;
		case _IOLBUF: stream->state |= STREAM_LINESYNC; break;
		case _IOFBUF: stream->state &= ~STREAM_LINESYNC; break;
		default: return EOF;
	}
	if (size < 0) return EOF;
	_sync(stream);
	_buffer(stream, buffer, size);
}

