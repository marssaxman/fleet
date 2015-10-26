#include <string.h>
#include <stdio.h>

int fputs(const char *str, FILE *stream)
{
	size_t bytes = strlen(str);
	return fwrite(str, sizeof(char), bytes, stream) == bytes? bytes: EOF;
}

