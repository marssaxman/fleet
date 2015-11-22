#include "stream.h"

void clearerr(FILE *stream)
{
	stream->state = 0;
}

