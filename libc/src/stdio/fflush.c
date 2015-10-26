#include "internal/bufio.h"

int fflush(FILE *stream)
{
	return _flush(stream);
}

