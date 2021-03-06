// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

int strncmp(const char *l, const char *r, size_t n)
{
	while (n--) {
		unsigned char lch = *(unsigned char*)l++;
		unsigned char rch = *(unsigned char*)r++;
		if (lch != rch) return lch - rch;
		if (!lch) break;
	}
	return 0;
}

#ifdef TESTSUITE
TESTSUITE(strncmp) {
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	// make sure strncmp does what strcmp does when the size is adequate
	CHECK(0 == strncmp("", "", 40));
	CHECK(0 == strncmp("a", "a", 40));
	CHECK(0 == strncmp("abc", "abc", 40));
	CHECK(0 > strncmp("abc", "abcd", 40));
	CHECK(0 < strncmp("abcd", "abc", 40));
	CHECK(0 > strncmp("abcd", "abce", 40));
	CHECK(0 < strncmp("abce", "abcd", 40));
	CHECK(0 < strncmp("a\x42", "a", 40));
	CHECK(0 < strncmp("a\x42", "a\x02", 40));
	CHECK(0 == strncmp(upper, upper, 40));
	CHECK(0 < strncmp(lower, upper, 40));
	CHECK(0 > strncmp(upper, lower, 40));
	CHECK(0 > strncmp("foo", "foobar", 40));
	CHECK(0 < strncmp("foo", "fobbed", 40));
	// now check behavior when size is less than or equal to string length
	CHECK(0 == strncmp("flibbet", "flipper", 3));
	CHECK(0 == strncmp("barrister", "bar", 3));
	CHECK(0 > strncmp("flip", "flit", 4));
	CHECK(0 == strncmp("hopeless", "mismatch", 0));
}
#endif

