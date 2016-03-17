// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

char *strchr(const char *str, int ch)
{
	while (*str && *str != ch) {
		str++;
	}
	return (*str == ch) ? (char*)str : NULL;
}

#ifdef TESTSUITE
TESTSUITE(strchr) {
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(&lower[0] == strchr(lower, 'a'));
	CHECK(&lower[3] == strchr(lower, 'd'));
	char buf[] = "abcabcabc";
	CHECK(&buf[1] == strchr(buf, 'b'));
	CHECK(&lower[25] == strchr(lower, 'z'));
	CHECK(0 == strchr(lower, 'A'));
	CHECK(&lower[26] == strchr(lower, '\0'));
	CHECK(0 == strchr("", 'X'));
	char dummy[] = "";
	CHECK(dummy == strchr(dummy, '\0'));
}
#endif


