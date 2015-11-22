#include "stream.h"

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	if (!stream) return -1;
	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF) return -1;
	if (mode == _IONBF) {
		// The standard says we will ignore the buffer and size parameters
		// when configuring a stream for no buffer.
		buffer = NULL;
		size = 0;
	}
	// Functionally speaking, size = 0 also means _IONBF regardless of mode.
	// If buffer is NULL and size > 0, we are supposed to malloc a buffer,
	// but we don't have malloc yet so we can't.
	if (buffer == NULL && size > 0) {
		return -1;
	}
	// Configure the buffer.
	stream->buf_addr = buffer;
	stream->buf_size = size;
	stream->buf_count = 0;
	// Set the line sync flag, which is an alternate limit on buffer size.
	if (mode == _IOLBF) {
		stream->state |= STREAM_LINESYNC;
	} else {
		stream->state &= ~STREAM_LINESYNC;
	}
}

