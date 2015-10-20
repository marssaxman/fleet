#include <string.h>

char *strcat(char *dest, const char *src)
{
	char *out = dest;
	while (*dest) dest++;
	while (*dest++ = *src++) {}
	return out;
}

