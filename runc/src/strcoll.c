#include <string.h>

int strcoll(const char *l, const char *r)
{
	return strcmp(l, r);
}

TESTBEGIN
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(0 == strcoll("", ""));
	CHECK(0 == strcoll("a", "a"));
	CHECK(0 == strcoll("abc", "abc"));
	CHECK(0 > strcoll("abc", "abcd"));
	CHECK(0 < strcoll("abcd", "abc"));
	CHECK(0 > strcoll("abcd", "abce"));
	CHECK(0 < strcoll("abce", "abcd"));
	CHECK(0 < strcoll("a\x42", "a"));
	CHECK(0 < strcoll("a\x42", "a\x02"));
	CHECK(0 == strcoll(upper, upper));
	CHECK(0 < strcoll(lower, upper));
	CHECK(0 > strcoll(upper, lower));
	CHECK(0 > strcoll("foo", "foobar"));
	CHECK(0 < strcoll("foo", "fobbed"));
TESTEND

