#include <stdio.h>
#include <errno.h>

int rename(const char *oldname, const char *newname)
{
	errno = ENOENT;
	return -1;
}
