#include <stdio.h>
#include <errno.h>

FILE *fopen(const char *filename, const char *mode)
{
	// No kernel support; always fails.
	errno = ENOENT;
	return NULL;
}
