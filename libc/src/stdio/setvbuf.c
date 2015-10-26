#include "internal/bufio.h"
#include "internal/stream.h"

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	if (mode == _IONBUF) {
		buffer = NULL;
		size = 0;
	} else if (buffer == NULL) {
		// We are supposed to allocate a buffer, but we don't have malloc.
		return -1;
	}
	_buffer(stream, buffer, size);
	if (mode == _IOLBUF) {
		stream->state |= STREAM_LINESYNC;
	} else {
		stream->state &= ~STREAM_LINESYNC;
	}
}

