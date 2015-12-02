// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

int memcmp(const void *lptr, const void *rptr, size_t n)
{
    const unsigned char *l=lptr, *r=rptr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

#ifdef TESTSUITE
TESTSUITE(memcmp) {
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(0 != memcmp(upper, lower, 26));
	CHECK(0 == memcmp(upper, upper, 26));
	CHECK(0 == memcmp(&upper[1], &upper[1], 20));
	char c[] = "ABCABCDEFDEF";
	CHECK(0 == memcmp(&c[0], &c[3], 3));
	CHECK(0 == memcmp(&c[6], &c[9], 3));
	char d[] = "123456789123456789";
	CHECK(0 == memcmp(&d[0], &d[9], 9));
}
#endif

