#include <string.h>

char *strncat(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (*dest) dest++;
	while (n-- && (*dest++ = *src++)) {
		if (0 == n) {
			*dest = '\0';
		}
	}
	return out;
}

