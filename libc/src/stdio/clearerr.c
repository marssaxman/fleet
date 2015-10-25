#include "internal/bufio.h"
#include "internal/stream.h"

void clearerr(FILE *stream)
{
	stream->state = 0;
}

