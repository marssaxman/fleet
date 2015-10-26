#include "internal/stream.h"
#include <sys/socket.h>
#include <errno.h>

size_t fwrite(const void *src, size_t size, size_t count, FILE *stream)
{
	size_t bytes = size*count;
	if (0 == bytes) return 0;
	int ret = write(stream->id, src, bytes);
	if (ret < bytes) {
		errno = -ret;
		stream->state |= STREAM_ERR;
		return 0;
	}
	return ret / size;
}


