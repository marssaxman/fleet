#include "internal/bufio.h"

int fputc(int character, FILE *stream)
{
	return _write(stream, &character, 1) > 0? character: EOF;
}

