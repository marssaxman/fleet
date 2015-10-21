#include <string.h>

void *memchr(const void *buf, int ch, size_t n)
{
	const unsigned char *src = buf;
	while (n--) {
		if (*src == (unsigned char)ch) {
			return (void*)src;
		}
		src++;
	}
	return NULL;
}

TESTBEGIN
	// TODO
TESTEND
