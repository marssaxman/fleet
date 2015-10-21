#include <string.h>
#include "testsuite.h"

size_t strxfrm(char *dest, const char *src, size_t n)
{
	size_t out = 0;
	while (n--) {
		if (*dest++ = *src++) {
			++out;
		} else {
			return out;
		}
	}
	while (*src) {
		++src;
		++out;
	}
	return out;
}

TESTSUITE {
	// TODO
}

