#include <string.h>
#include "testsuite.h"

void *memmove(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;
	if (d > s && d < s+n) {
		// The dest buffer overlaps the end of the src, so copy in reverse.
		d += n;
		s += n;
		while (n--) {
			*--d = *--s;
		}
	} else {
		// The dest buffer precedes the src, or there is no overlap.
		while (n--) {
			*d++ = *s++;
		}
	}
	return dest;
}

TESTSUITE {
	static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
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

