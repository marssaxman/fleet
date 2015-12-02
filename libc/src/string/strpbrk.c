// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

char *strpbrk(const char *str, const char *set)
{
	while (*str) {
		for (const char *c = set; *c; c++) {
			if (*str == *c) return (char*)str;
		}
		str++;
	}
	return NULL;
}

#ifdef TESTSUITE
TESTSUITE(strpbrk) {
	char buf[] = "This is a message from the beyond.";
	char common[] = "ent";
	const char *ret = strpbrk(buf, common);
	CHECK_STR(ret, "essage from the beyond.", 33-(ret-buf));
	ret = strpbrk(++ret, common);
	CHECK_STR(ret, "e from the beyond.", 33-(ret-buf));
	ret = strpbrk(++ret, common);
	CHECK_STR(ret, "the beyond.", 33-(ret-buf));
	ret = strpbrk(++ret, common);
	CHECK_STR(ret, "e beyond.", 33-(ret-buf));
	ret = strpbrk(++ret, common);
	CHECK_STR(ret, "eyond.", 33-(ret-buf));
	ret = strpbrk(++ret, common);
	CHECK_STR(ret, "nd.", 33-(ret-buf));
	CHECK(0 == strpbrk(++ret, common));
}
#endif

