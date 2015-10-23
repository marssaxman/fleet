#include "internal/stdio.h"
#include "internal/stream.h"

void clearerr(FILE *stream)
{
	stream->state = 0;
}

