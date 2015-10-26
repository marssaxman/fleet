#include <stdio.h>

void setbuf(FILE *stream, char *buffer)
{
	setvbuf(stream, buffer, buffer?_IOFBUF:_IONBUF, BUFSIZ);
}

