#include "internal/stdio.h"

int fputc(int character, FILE *stream)
{
	return _write(stream, &character, 1) > 0? character: EOF;
}

