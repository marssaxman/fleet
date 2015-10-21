#include <string.h>
#include "testsuite.h"

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

TESTSUITE {
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

