#include <string.h>
#include "internal/testsuite.h"

char *strcat(char *dest, const char *src)
{
	char *out = dest;
	while (*dest) dest++;
	while ((*dest++ = *src++)) {}
	return out;
}

TESTSUITE {
	char buf[64];
	strcpy(buf, "hello ");
	CHECK(buf == strcat(buf, "my name is "));
	CHECK_STR(buf, "hello my name is ", 64);
	CHECK(buf == strcat(buf, "inigo montoya"));
	CHECK_STR(buf, "hello my name is inigo montoya", 64);
}


