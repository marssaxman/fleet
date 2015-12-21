#include <stdio.h>
#include <errno.h>

int fseek(FILE *stream, long int offset, int origin)
{
	errno = ESPIPE;
	return -1;
}
