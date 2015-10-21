#include <string.h>
#include "testsuite.h"

// implemented in startc, but we'll test it here anyway

TESTBEGIN
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	char buf[26];
	for (int i = 0; i < 26; ++i) {
		// fill the beginning of the buffer with uppercase chars and the end
		// of the buffer with lowercase chars
		memcpy(buf, upper, i);
		memcpy(&buf[i], lower, 26-i);
		// verify that we have copied the text we expected
		CHECK_MEM(buf, upper, i);
		CHECK_MEM(&buf[i], lower, 26-i);
		// do the same, but start from the end of each source buffer
		memcpy(buf, &upper[26-i], i);
		memcpy(&buf[i], &lower[i], 26-i);
		// verify that we got the data we expected
		CHECK_MEM(buf, &upper[26-i], i);
		CHECK_MEM(&buf[i], &lower[i], 26-i);
	}
TESTEND

