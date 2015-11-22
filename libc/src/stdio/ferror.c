#include <stdio.h>
#include "stream.h"

int ferror(FILE *stream)
{
	return stream->state & STREAM_ERR;
}


