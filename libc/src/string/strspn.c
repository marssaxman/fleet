// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

size_t strspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c == '\0') break;
		str++;
	}
	return str - start;
}

#ifdef TESTSUITE
TESTSUITE(strspn) {
	CHECK(5 == strspn("never better the wetter weather", "brevent"));
	const char str[] = "73fcba73";
	const char keys[] = "1234567890";
	CHECK(2 == strspn(str, keys));
	CHECK(0 == strspn(str, "HELLO"));
	CHECK(0 == strspn(str, ""));
	CHECK(3 == strspn("129th", "1234567890"));
}
#endif

