#include <string.h>

char *strrchr(const char *str, int ch)
{
	const char *out = NULL;
	do {
		if (*str == ch) {
			out = str;
		}
	} while (*str++);
	return (char*)out;
}

