#include <stdio.h>
#include "stream.h"

int feof(FILE *stream)
{
	return stream->state & STREAM_EOF;
}

