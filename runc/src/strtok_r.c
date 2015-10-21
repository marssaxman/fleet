#include <string.h>
#include "testsuite.h"

char *strtok_r(char *str, const char *delims, char **lasts)
{
	char *sbegin, *send;
	sbegin = str ? str : *lasts;
	sbegin += strspn(sbegin, delims);
	if (*sbegin == '\0') {
		*lasts = "";
		return NULL;
	}
	send = sbegin + strcspn(sbegin, delims);
	if (*send != '\0') {
		*send++ = '\0';
	}
	*lasts = send;
	return sbegin;
}

TESTSUITE {
	// TODO
}


