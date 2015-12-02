// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

size_t strxfrm(char *dest, const char *src, size_t n)
{
	size_t out = 0;
	while (n--) {
		if ((*dest++ = *src++)) {
			++out;
		} else {
			return out;
		}
	}
	while (*src) {
		++src;
		++out;
	}
	return out;
}

#ifdef TESTSUITE
TESTSUITE(strxfrm) {
	char a[64];
	char b[64];
	strcpy(a, "What goes around must come around");
	int ret = strxfrm(b, a, strlen(a)+1);
	CHECK_STR(a, b, 64);
	CHECK(ret == strlen(a));
	a[0] = '\0';
	strcpy(b, "Always pleasure, my friend. Any time.");
	ret = strxfrm(b, a, strlen(a)+1);
	CHECK_STR(b, "", 64);
	CHECK(ret == strlen(a));
	strcpy(b, "Another fine product from the nonsense factory");
	ret = strxfrm(NULL, b, 0);
	CHECK(ret == strlen(b));
}
#endif

