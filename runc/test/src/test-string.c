#include "test-string.h"
#include "check.h"
#include "debug-write.h"
#include <string.h>
#include <stdbool.h>

static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char lower[] = "abcdefghijklmnopqrstuvwxyz";

static void test_memcmp()
{
	CHECK(0 != memcmp(upper, lower, 26));
	CHECK(0 == memcmp(upper, upper, 26));
	CHECK(0 == memcmp(&upper[1], &upper[1], 20));
	char c[] = "ABCABCDEFDEF";
	CHECK(0 == memcmp(&c[0], &c[3], 3));
	CHECK(0 == memcmp(&c[6], &c[9], 3));
	char d[] = "123456789123456789";
	CHECK(0 == memcmp(&d[0], &d[9], 9));
}

static void test_memcpy()
{
	char buf[26];
	for (int i = 0; i < 26; ++i) {
		// fill the beginning of the buffer with uppercase chars and the end
		// of the buffer with lowercase chars
		memcpy(buf, upper, i);
		memcpy(&buf[i], lower, 26-i);
		// verify that we have copied the text we expected
		CHECK_MEM(buf, upper, i);
		CHECK_MEM(&buf[i], lower, 26-i);
		// do the same, but start from the end of each source buffer
		memcpy(buf, &upper[26-i], i);
		memcpy(&buf[i], &lower[i], 26-i);
		// verify that we got the data we expected
		CHECK_MEM(buf, &upper[26-i], i);
		CHECK_MEM(&buf[i], &lower[i], 26-i);
	}
}

static void test_memmove()
{
	char buf[26];
	// start with a few simple checks
	memmove(buf, lower, 26);
	CHECK_MEM(buf, lower, 26);
	memmove(buf, upper, 16);
	CHECK_MEM(buf, "ABCDEFGHIJKLMNOPqrstuvwxyz", 26);
	memmove(buf, lower, 3);
	CHECK_MEM(buf, "abcDEFGHIJKLMNOPqrstuvwxyz", 26);
	memmove(&buf[23], upper, 3);
	CHECK_MEM(buf, "abcDEFGHIJKLMNOPqrstuvwABC", 26);
	memmove(&buf[19], &upper[3], 7);
	CHECK_MEM(buf, "abcDEFGHIJKLMNOPqrsDEFGHIJ", 26);
	// copy from source to dest at all alignments and lengths
	for (int start = 0; start < 26; ++start) {
		for (int len = 0; len < (26-start); ++len) {
			// fill the buffer, then overwrite part of it
			memmove(buf, upper, 26);
			memmove(&buf[start], lower, len);
			// replicate this result using memcpy and fresh data
			char scratch[26];
			memcpy(scratch, upper, 26);
			memcpy(&scratch[start], lower, len);
			CHECK_MEM(buf, scratch, 26);
		}
	}
	// simple checks for overlapping copy behavior
	memmove(buf, lower, 26);
	memmove(&buf[3], buf, 16);
	CHECK_MEM(buf, "abcabcdefghijklmnoptuvwxyz", 26);
	memmove(buf, lower, 26);
	memmove(&buf[3], buf, 5);
	CHECK_MEM(buf, "abcabcdeijklmnopqrstuvwxyz", 26);
	memmove(buf, lower, 26);
	memmove(buf, &buf[3], 16);
	CHECK_MEM(buf, "defghijklmnopqrsqrstuvwxyz", 26);
	memmove(buf, lower, 26);
	memmove(buf, &buf[3], 5);
	CHECK_MEM(buf, "defghfghijklmnopqrstuvwxyz", 26);
	// copy at all alignments and lengths
	for (int start = 0; start < 26; ++start) {
		for (int len = 0; len < (26-start); ++len) {
			// reset the buffer, then copy it onto itself
			memmove(buf, lower, 26);
			memmove(&buf[start], buf, len);
			// make an equivalent buffer from originals with memcpy
			char scratch[26];
			memcpy(scratch, lower, 26);
			memcpy(&scratch[start], lower, len);
			CHECK_MEM(buf, scratch, 26);
			// do it again the opposite direction
			memmove(buf, lower, 26);
			memmove(buf, &buf[start], len);
			// make an equivalent buffer via memcpy
			memcpy(scratch, lower, 26);
			memcpy(scratch, &lower[start], len);
			CHECK_MEM(buf, scratch, 26);
		}
	}
}

static void test_memset()
{
	char buf[26];
	static const char naughts[26] = "00000000000000000000000000";
	static const char crosses[26] = "XXXXXXXXXXXXXXXXXXXXXXXXXX";
	// simple tests to start with
	memset(buf, '0', 26);
	CHECK_MEM(buf, naughts, 26);
	memset(buf, 'X', 26);
	CHECK_MEM(buf, crosses, 26);
	// all lengths, all offsets
	for (int start = 0; start < 26; ++start) {
		for (int len = 0; len < (26-start); ++len) {
			// fill the buffer using memset
			memset(buf, '0', 26);
			memset(&buf[start], 'X', len);
			// compare to the results we get with memcpy
			char scratch[26];
			memcpy(scratch, naughts, 26);
			memcpy(&scratch[start], crosses, len);
			CHECK_MEM(buf, scratch, 26);
		}
	}
}

static void test_strcpy()
{
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

static void test_strncpy()
{
	char buf[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	char *ret = strncpy(buf, upper, 30);
	CHECK(ret == buf);
	CHECK_STR(buf, upper, 40);
	CHECK_MEM(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0\0\0\0XXXXXXXXXX", 40);
	strncpy(buf, lower, 10);
	CHECK_MEM(buf, "abcdefghijKLMNOPQRSTUVWXYZ\0", 27);
}

static void test_strcmp()
{
	CHECK(0 == strcmp("", ""));
	CHECK(0 == strcmp("a", "a"));
	CHECK(0 == strcmp("abc", "abc"));
	CHECK(0 > strcmp("abc", "abcd"));
	CHECK(0 < strcmp("abcd", "abc"));
	CHECK(0 > strcmp("abcd", "abce"));
	CHECK(0 < strcmp("abce", "abcd"));
	CHECK(0 < strcmp("a\x42", "a"));
	CHECK(0 < strcmp("a\x42", "a\x02"));
	CHECK(0 == strcmp(upper, upper));
	CHECK(0 < strcmp(lower, upper));
	CHECK(0 > strcmp(upper, lower));
	CHECK(0 > strcmp("foo", "foobar"));
	CHECK(0 < strcmp("foo", "fobbed"));
}

static void test_strncmp()
{
	// make sure strncmp does what strcmp does when the size is adequate
	CHECK(0 == strncmp("", "", 40));
	CHECK(0 == strncmp("a", "a", 40));
	CHECK(0 == strncmp("abc", "abc", 40));
	CHECK(0 > strncmp("abc", "abcd", 40));
	CHECK(0 < strncmp("abcd", "abc", 40));
	CHECK(0 > strncmp("abcd", "abce", 40));
	CHECK(0 < strncmp("abce", "abcd", 40));
	CHECK(0 < strncmp("a\x42", "a", 40));
	CHECK(0 < strncmp("a\x42", "a\x02", 40));
	CHECK(0 == strncmp(upper, upper, 40));
	CHECK(0 < strncmp(lower, upper, 40));
	CHECK(0 > strncmp(upper, lower, 40));
	CHECK(0 > strncmp("foo", "foobar", 40));
	CHECK(0 < strncmp("foo", "fobbed", 40));
	// now check behavior when size is less than or equal to string length
	CHECK(0 == strncmp("flibbet", "flipper", 3));
	CHECK(0 == strncmp("barrister", "bar", 3));
	CHECK(0 > strncmp("flip", "flit", 4));
	CHECK(0 == strncmp("hopeless", "mismatch", 0));
}

static void test_strcoll()
{
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
}

static void test_strcat()
{
	char buf[64];
	strcpy(buf, "hello ");
	CHECK(buf == strcat(buf, "my name is "));
	CHECK_STR(buf, "hello my name is ", 64);
	CHECK(buf == strcat(buf, "inigo montoya"));
	CHECK_STR(buf, "hello my name is inigo montoya", 64);
}

static void test_strncat()
{
	char buf[64];
	memset(buf, 'X', 64);
	strcpy(buf, "around ");
	// copy with more space than necessary
	CHECK(buf == strncat(buf, "the world ", 12));
	CHECK_STR(buf, "around the world ", 64);
	CHECK_MEM(buf, "around the world \0XXXXXX", 24);
	CHECK(buf == strncat(buf, "in eighty days", 8));
	CHECK_MEM(buf, "around the world in eight\0XXXXXX", 32);
}

static void test_strchr()
{
	CHECK(&lower[0] == strchr(lower, 'a'));
	CHECK(&lower[3] == strchr(lower, 'd'));
	char buf[] = "abcabcabc";
	CHECK(&buf[1] == strchr(buf, 'b'));
	CHECK(&lower[25] == strchr(lower, 'z'));
	CHECK(0 == strchr(lower, 'A'));
	CHECK(&lower[26] == strchr(lower, '\0'));
	CHECK(0 == strchr("", 'X'));
	char dummy[] = "";
	CHECK(dummy == strchr(dummy, '\0'));
}

static void test_strrchr()
{
	CHECK(&lower[0] == strrchr(lower, 'a'));
	CHECK(&lower[3] == strrchr(lower, 'd'));
	char buf[] = "abcabcabc";
	CHECK(&buf[7] == strrchr(buf, 'b'));
	CHECK(&lower[25] == strrchr(lower, 'z'));
	CHECK(0 == strrchr(lower, 'A'));
	CHECK(&lower[26] == strrchr(lower, '\0'));
	CHECK(0 == strrchr("", 'X'));
	char dummy[] = "";
	CHECK(dummy == strrchr(dummy, '\0'));
}

static void test_strstr()
{
}

static void test_memchr()
{
}

static void test_strspn()
{
}

static void test_strcspn()
{
}

static void test_strpbrk()
{
}

static void test_strtok()
{
}

static void test_strtok_r()
{
}

static void test_strlen()
{
}

static void test_strxfrm()
{
}

void test_string()
{
	// test startc implementations first, since these functions will come in
	// handy when testing the other functions
	test_memcmp();
	test_memcpy();
	test_memmove();
	test_memset();
	// test the string functions which are implemented in runc itself
	test_strcpy();
	test_strncpy();
	test_strcmp();
	test_strncmp();
	test_strcoll();
	test_strcat();
	test_strncat();
	test_strchr();
	test_strrchr();
	test_strstr();
	test_memchr();
	test_strspn();
	test_strcspn();
	test_strpbrk();
	test_strtok();
	test_strtok_r();
	test_strlen();
	test_strxfrm();
}

