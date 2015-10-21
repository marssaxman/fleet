#include <string.h>

char *strtok(char *str, const char *delims)
{
	static char *save = "";
	return strtok_r(str, delims, &save);
}

TESTBEGIN
	// TODO
TESTEND

