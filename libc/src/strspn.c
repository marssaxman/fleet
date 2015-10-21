#include <string.h>
#include "testsuite.h"

size_t strspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c == '\0') break;
		str++;
	}
	return str - start;
}

TESTSUITE {
	CHECK(5 == strspn("never better the wetter weather", "brevent"));
	const char str[] = "73fcba73";
	const char keys[] = "1234567890";
	CHECK(2 == strspn(str, keys));
	CHECK(0 == strspn(str, "HELLO"));
	CHECK(0 == strspn(str, ""));
	CHECK(3 == strspn("129th", "1234567890"));
}

