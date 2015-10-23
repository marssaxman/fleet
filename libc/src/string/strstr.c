#include <string.h>

char *strstr(const char *haystack, const char *needle)
{
	if (!*needle) return (char*)haystack;
	if (!*haystack) return NULL;
	size_t n = strlen(needle);
	for (; (haystack = strchr(haystack, *needle)); haystack++) {
		if (!strncmp(haystack, needle, n)) {
			return (char*)haystack;
		}
	}
	return NULL;
}

#ifdef TESTSUITE
TESTSUITE(strstr) {
	const char fizz[64] = "Hello world, how are you doing this fine evening?";
	CHECK(&fizz[17] == strstr(fizz, "are you"));
	CHECK(NULL == strstr(fizz, "Hello world!"));
	CHECK(&fizz[14] == strstr(&fizz[14], ""));
	CHECK(NULL == strstr("", "what is the deal?"));
	const char empty[1] = "";
	CHECK(empty == strstr(empty, ""));
}
#endif



