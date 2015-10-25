#include <string.h>
#include "internal/stdio.h"

int fputs(const char *str, FILE *stream)
{
	size_t bytes = strlen(str);
	size_t n = _write(stream, str, bytes);
	return (n == bytes)? n: EOF;
}

