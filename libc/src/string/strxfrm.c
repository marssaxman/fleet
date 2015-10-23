#include <string.h>
#include "internal/testsuite.h"

size_t strxfrm(char *dest, const char *src, size_t n)
{
	size_t out = 0;
	while (n--) {
		if ((*dest++ = *src++)) {
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
	char a[64];
	char b[64];
	strcpy(a, "What goes around must come around");
	int ret = strxfrm(b, a, strlen(a)+1);
	CHECK_STR(a, b, 64);
	CHECK(ret == strlen(a));
	a[0] = '\0';
	strcpy(b, "Always pleasure, my friend. Any time.");
	ret = strxfrm(b, a, strlen(a)+1);
	CHECK_STR(b, "", 64);
	CHECK(ret == strlen(a));
	strcpy(b, "Another fine product from the nonsense factory");
	ret = strxfrm(NULL, b, 0);
	CHECK(ret == strlen(b));
}

