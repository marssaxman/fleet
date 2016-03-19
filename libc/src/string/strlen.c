// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

size_t strlen(const char *str)
{
	const char *p = str;
	while (*p != '\0') {
		p++;
	}
	return (size_t)(p - str);
}

#ifdef TESTSUITE
TESTSUITE(strlen) {
	const char buf[] = "BIGbrownBOXESofBEETLEboots";
	CHECK(0 == strlen(""));
	CHECK(26 == strlen(buf));
	CHECK(0 == strlen(&buf[26]));
	const char hole[] = "This is a string\0with a hole in it";
	CHECK(16 == strlen(hole));
	CHECK(17 == strlen(&hole[17]));
}
#endif


