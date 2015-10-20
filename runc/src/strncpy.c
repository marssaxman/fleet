#include <string.h>

char *strncpy(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (n--) {
		*dest++ = *src;
		if (*src) src++;
	}
	return out;
}

