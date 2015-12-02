// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

char *strncat(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (*dest) dest++;
	while (n-- && (*dest++ = *src++)) {
		if (0 == n) {
			*dest = '\0';
		}
	}
	return out;
}

#ifdef TESTSUITE
TESTSUITE(strncat) {
	char buf[64];
	memset(buf, 'X', 64);
	strcpy(buf, "around ");
	// copy with more space than necessary
	CHECK(buf == strncat(buf, "the world ", 12));
	CHECK_STR(buf, "around the world ", 64);
	CHECK_MEM(buf, "around the world \0XXXXXX", 24);
	CHECK(buf == strncat(buf, "in eighty days", 8));
	CHECK_MEM(buf, "around the world in eight\0XXXXXX", 32);
}
#endif


