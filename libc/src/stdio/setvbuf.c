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
	stream->buf_pos = stream->buf_addr = buffer;
	stream->buf_size = size;
	stream->buf_end = buffer + size;
	if (mode == _IOLBUF) {
		stream->state |= STREAM_LINESYNC;
	} else {
		stream->state &= ~STREAM_LINESYNC;
	}
}

