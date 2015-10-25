#include "internal/stdio.h"

int fflush(FILE *stream)
{
	return _sync(stream);
}

