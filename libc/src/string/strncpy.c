#include <string.h>

char *strncpy(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (n--) {
		*dest++ = *src;
		if (*src) src++;
	}
	return out;
}

#ifdef TESTSUITE
TESTSUITE(strncpy) {
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	char buf[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	char *ret = strncpy(buf, upper, 30);
	CHECK(ret == buf);
	CHECK_STR(buf, upper, 40);
	CHECK_MEM(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0XXXXXXXXXX", 40);
	strncpy(buf, lower, 10);
	CHECK_MEM(buf, "abcdefghijKLMNOPQRSTUVWXYZ\0", 27);
}
#endif

