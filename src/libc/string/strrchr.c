// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

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

#ifdef TESTSUITE
TESTSUITE(strrchr) {
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
}
#endif


