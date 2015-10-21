#include <string.h>
#include "testsuite.h"

// memcmp is implemented in startc, but we'll test it here anyway

TESTBEGIN
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(0 != memcmp(upper, lower, 26));
	CHECK(0 == memcmp(upper, upper, 26));
	CHECK(0 == memcmp(&upper[1], &upper[1], 20));
	char c[] = "ABCABCDEFDEF";
	CHECK(0 == memcmp(&c[0], &c[3], 3));
	CHECK(0 == memcmp(&c[6], &c[9], 3));
	char d[] = "123456789123456789";
	CHECK(0 == memcmp(&d[0], &d[9], 9));
TESTEND

