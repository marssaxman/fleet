#include <string.h>

size_t strlen(const char *str)
{
	const char *p = str;
	while (*p != '\0') {
		p++;
	}
	return (size_t)(p - str);
}

TESTBEGIN
	// TODO
TESTEND

