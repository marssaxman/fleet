// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

void *memset(void *dest, int c, size_t n)
{
	unsigned char *d = dest;
	while (n--) {
		*d++ = c;
	}
	return dest;
}

#ifdef TESTSUITE
TESTSUITE(memset) {
	char buf[26];
	static const char naughts[26] = "00000000000000000000000000";
	static const char crosses[26] = "XXXXXXXXXXXXXXXXXXXXXXXXXX";
	// simple tests to start with
	memset(buf, '0', 26);
	CHECK_MEM(buf, naughts, 26);
	memset(buf, 'X', 26);
	CHECK_MEM(buf, crosses, 26);
	// all lengths, all offsets
	for (int start = 0; start < 26; ++start) {
		for (int len = 0; len < (26-start); ++len) {
			// fill the buffer using memset
			memset(buf, '0', 26);
			memset(&buf[start], 'X', len);
			// compare to the results we get with memcpy
			char scratch[26];
			memcpy(scratch, naughts, 26);
			memcpy(&scratch[start], crosses, len);
			CHECK_MEM(buf, scratch, 26);
		}
	}
}
#endif

