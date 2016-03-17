// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <string.h>

char *strtok(char *str, const char *delims)
{
	static char *save = "";
	char **lasts = &save;

	char *sbegin, *send;
	sbegin = str ? str : *lasts;
	sbegin += strspn(sbegin, delims);
	if (*sbegin == '\0') {
		*lasts = "";
		return NULL;
	}
	send = sbegin + strcspn(sbegin, delims);
	if (*send != '\0') {
		*send++ = '\0';
	}
	*lasts = send;
	return sbegin;
}

#ifdef TESTSUITE
TESTSUITE(strtok) {
	char buf[64];
	strcpy(buf, "This, that, and the other, you know?");
	CHECK_STR("This", strtok(buf, ","), 32);
	CHECK_STR(" that", strtok(NULL, ","), 32);
	CHECK_STR(" and the other", strtok(NULL, ","), 32);
	CHECK_STR(" you know?", strtok(NULL, ","), 32);
	CHECK(0 == strtok(NULL, ","));
	strcpy(buf, "- This, a sample string.");
	char *pch = strtok(buf, " ,.-");
	CHECK(0 != pch);
	char *vals[] = {"This", "a", "sample", "string"};
	int valdex = 0;
	while (pch) {
		CHECK_STR(pch, vals[valdex++], 64);
		pch = strtok(NULL, " ,.-");
	}
}
#endif


