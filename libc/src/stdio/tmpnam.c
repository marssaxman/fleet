#include <stdio.h>
#include <errno.h>

char *tmpnam(char *s)
{
	errno = EROFS;
	return NULL;
}

