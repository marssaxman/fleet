#include <stdio.h>

void setbuf(FILE *stream, char *buffer)
{
	setvbuf(stream, buffer, buffer?_IOFBUF:_IONBUF, buffer?BUFSIZ:0);
}

