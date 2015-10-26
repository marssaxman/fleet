#include <stdio.h>

int fgetc(FILE *stream)
{
	char out = '\0';
	return fread(&out, sizeof(char), 1, stream) == 1? out: EOF;
}

