#include "internal/stdio.h"
#include "internal/stream.h"

int feof(FILE *stream)
{
	return stream->state & FILE_EOF;
}

