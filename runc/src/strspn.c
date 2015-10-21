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
	// TODO
}

