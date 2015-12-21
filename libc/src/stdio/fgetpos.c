#include <stdio.h>
#include <errno.h>

int fgetpos(FILE *stream, fpos_t *pos)
{
	errno = ESPIPE;
	return -1;
}

