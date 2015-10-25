#include <stdio.h>
#include "internal/stream.h"

int ferror(FILE *stream)
{
	return stream->state & STREAM_ERR;
}


