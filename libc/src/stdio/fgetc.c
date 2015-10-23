#include "internal/stdio.h"

int fgetc(FILE *stream)
{
	char out = '\0';
	return _read(stream, &out, 1) == 1? out: EOF;
}

