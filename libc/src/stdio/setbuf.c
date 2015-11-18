#include <stdio.h>

void setbuf(FILE *stream, char *buffer)
{
	setvbuf(stream, buffer, buffer?_IOFBF:_IONBF, buffer?BUFSIZ:0);
}

