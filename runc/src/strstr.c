#include <string.h>
#include "testsuite.h"

char *strstr(const char *haystack, const char *needle)
{
	if (!*needle) return (char*)haystack;
	if (!*haystack) return NULL;
	size_t n = strlen(needle);
	for (;haystack = strchr(haystack, *needle); haystack++) {
		if (!strncmp(haystack, needle, n)) {
			return (char*)haystack;
		}
	}
	return NULL;
}

TESTSUITE {
	// TODO
}


