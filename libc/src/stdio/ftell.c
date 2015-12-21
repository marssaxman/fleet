#include <stdio.h>
#include <errno.h>

long int ftell(FILE *stream)
{
	errno = ESPIPE;
	return -1;
}

