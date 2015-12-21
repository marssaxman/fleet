#include <stdio.h>
#include <errno.h>

FILE *freopen(const char *filename, const char *mode, FILE *stream)
{
	// No kernel support; always fails.
	errno = ENOENT;
	return NULL;
}
