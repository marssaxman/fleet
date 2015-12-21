#include <stdio.h>
#include <errno.h>

FILE *tmpfile(void)
{
	errno = EROFS;
	return NULL;
}
