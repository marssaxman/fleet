#include <stdio.h>
#include "internal/stream.h"

int feof(FILE *stream)
{
	return stream->state & STREAM_EOF;
}

