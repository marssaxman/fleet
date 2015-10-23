#include "internal/stdio.h"
#include "internal/stream.h"

int ferror(FILE *stream)
{
	return stream->state & FILE_ERR;
}


