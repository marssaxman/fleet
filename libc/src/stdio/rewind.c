#include <stdio.h>
#include <errno.h>

void rewind(FILE *stream)
{
	errno = ESPIPE;
}


