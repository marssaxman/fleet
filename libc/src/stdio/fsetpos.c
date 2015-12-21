#include <stdio.h>
#include <errno.h>

int fsetpos(FILE *stream, const fpos_t *pos)
{
	errno = ESPIPE;
	return -1;
}

