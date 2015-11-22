#include "stream.h"
#include <sys/socket.h>

int fclose(FILE *stream)
{
	int flushret = fflush(stream);
	int closeret = close(stream->id);
	_exit_stream(stream);
	return (flushret || closeret)? EOF: 0;
}

