#include <string.h>
#include "internal/testsuite.h"

size_t strcspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c != '\0') break;
		str++;
	}
	return str - start;
}

TESTSUITE {
	const char str[] = "fcba73";
	const char keys[] = "1234567890";
	CHECK(4 == strcspn(str, keys));
	const char qbf[] = "The quick brown fox jumped over the lazy dog's back";
	CHECK(7 == strcspn(qbf, "c"));
	CHECK(12 == strcspn(qbf, "floop"));
	CHECK(strlen(qbf) == strcspn(qbf, "XYZ"));
	CHECK(strlen(qbf) == strcspn(qbf, ""));
	CHECK(0 == strcspn("", "hello again"));
}

