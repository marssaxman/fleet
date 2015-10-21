#include <string.h>

char *strrchr(const char *str, int ch)
{
	const char *out = NULL;
	do {
		if (*str == ch) {
			out = str;
		}
	} while (*str++);
	return (char*)out;
}

TESTBEGIN
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(&lower[0] == strrchr(lower, 'a'));
	CHECK(&lower[3] == strrchr(lower, 'd'));
	char buf[] = "abcabcabc";
	CHECK(&buf[7] == strrchr(buf, 'b'));
	CHECK(&lower[25] == strrchr(lower, 'z'));
	CHECK(0 == strrchr(lower, 'A'));
	CHECK(&lower[26] == strrchr(lower, '\0'));
	CHECK(0 == strrchr("", 'X'));
	char dummy[] = "";
	CHECK(dummy == strrchr(dummy, '\0'));
TESTEND

