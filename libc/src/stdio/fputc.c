#include <stdio.h>

int fputc(int character, FILE *stream)
{
	return fwrite(&character, sizeof(char), 1, stream) == 1? character: EOF;
}

