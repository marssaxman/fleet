// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

size_t strcspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c != '\0') break;
		str++;
	}
	return str - start;
}

#ifdef TESTSUITE
TESTSUITE(strcspn) {
	const char str[] = "fcba73";
	const char keys[] = "1234567890";
	CHECK(4 == strcspn(str, keys));
	const char qbf[] = "The quick brown fox jumped over the lazy dog's back";
	CHECK(7 == strcspn(qbf, "c"));
	CHECK(12 == strcspn(qbf, "floop"));
	CHECK(strlen(qbf) == strcspn(qbf, "XYZ"));
	CHECK(strlen(qbf) == strcspn(qbf, ""));
	CHECK(0 == strcspn("", "hello again"));
}
#endif

