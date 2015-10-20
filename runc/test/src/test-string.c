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
}

static void test_strncpy()
{
}

static void test_strcmp()
{
}

static void test_strncmp()
{
}

static void test_strcoll()
{
}

static void test_strcat()
{
}

static void test_strncat()
{
}

static void test_strchr()
{
}

static void test_strrchr()
{
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

