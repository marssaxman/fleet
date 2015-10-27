#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>

int fflush(FILE *stream)
{
	// If stream is NULL, we are supposed to flush *all* streams. Yoiks.
	if (!stream) return EOF;
	// For output streams, and update streams where the last operation was
	// output, flush must write any unwritten data from the buffer.
	// For input streams and update streams where the last operation was
	// input, the behavior is undefined. We will simply do nothing.
	if (stream->state & STREAM_READ) return 0;
	// Nor have we any work to do for a non-buffered stream.
	if (0 == stream->buf_size) return 0;
	// How much data does the buffer actually contain?
	size_t bytes = stream->buf_pos - stream->buf_addr;
	if (0 == bytes) return 0;
	// Write the buffered data out to the socket.
	int ret = write(stream->id, stream->buf_addr, bytes);
	// Clear the buffer and the write direction flag.
	stream->buf_pos = stream->buf_addr;
	stream->state &= ~STREAM_WRITE;
	// Report an error condition if the operation failed.
	if (ret < bytes) {
		errno = (ret < 0)? -ret: EOF;
		stream->state |= STREAM_ERR;
		return EOF;
	}
	// We're all good. 
	return 0;
}


