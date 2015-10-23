#include <string.h>
#include "internal/stdio.h"

int fputs(const char *str, FILE *stream)
{
	size_t n = _write(stream, str, strlen(str));
	return ferror(stream)? n: EOF;
}

