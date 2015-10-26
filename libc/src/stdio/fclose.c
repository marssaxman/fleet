#include "internal/stream.h"
#include <sys/socket.h>

int fclose(FILE *stream)
{
	int flushret = fflush(stream);
	int closeret = close(stream->id);
	return (flushret || closeret)? EOF: 0;
}

