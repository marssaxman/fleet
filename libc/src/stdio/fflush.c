#include "internal/bufio.h"

int fflush(FILE *stream)
{
	return _sync(stream);
}

