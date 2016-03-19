// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

char *strcpy(char *dest, const char *src)
{
	char *out = dest;
	while ((*dest++ = *src++)) {}
	return out;
}

#ifdef TESTSUITE
TESTSUITE(strcpy) {
	char str1[] = "Sample string";
	char str2[40];
	char str3[40];
	char *ret = strcpy(str2, str1);
	CHECK(ret == str2);
	CHECK_STR(str2, str1, 40);
	ret = strcpy(str3, "copy successful");
	CHECK(ret == str3);
	CHECK_STR(str3, "copy successful", 40);
	char buf[41] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	strcpy(&buf[5], "Bleep bloop");
	CHECK_MEM(buf, "XXXXXBleep bloop\0XXXXXXXXXXXXXXXXXXXXXXX", 40);
}
#endif

