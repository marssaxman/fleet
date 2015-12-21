#include <stdio.h>
#include <errno.h>

int remove(const char *filename)
{
	errno = ENOENT;
	return -1;
}
