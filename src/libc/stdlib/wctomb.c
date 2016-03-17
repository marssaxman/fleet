// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>
#include <errno.h>

int wctomb(char *dest, wchar_t wc)
{
	if (!dest) return 0;
	if (wc < 0x80) {
		// 0xxxxxxx: 7 bits
		dest[0] = wc;
		return 1;
	} else if (wc < 0x800) {
		// 110xxxxx 10xxxxxx: 11 bits
		dest[0] = ((wc >> 6) & 0x1F) | 0xC0;
		dest[1] = (wc & 0x3F) | 0x80;
		return 2;
	} else if (wc < 0x10000) {
		// 1110xxxx 10xxxxxx 10xxxxxx: 16 bits
		dest[0] = ((wc >> 12) & 0x0F) | 0xE0;
		dest[1] = ((wc >> 6) & 0x3F) | 0x80;
		dest[2] = (wc & 0x3F) | 0x80;
		return 3;
	} else if (wc < 0x110000) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx: 21 bits
		dest[0] = ((wc >> 18) & 0x07) | 0xF0;
		dest[1] = ((wc >> 12) & 0x3F) | 0x80;
		dest[2] = ((wc >> 6) & 0x3F) | 0x80;
		dest[3] = (wc & 0x3F) | 0x80;
		return 4;
	} else {
		return -1;
	}
}

#ifdef TESTSUITE
#include <string.h>
#include <limits.h>
TESTSUITE(wctomb_match) {
	struct check {
		const char *expected;
		wchar_t src;
	} examples[] = {
		{"", 0},
		{"a", L'a'},
		{"\xED\x9F\xBF", 0xD7FF},
		{"\xEE\x80\x80", 0xE000},
		{"\xEF\xBF\xBD", 0xFFFD},
		{"\xF4\x8F\xBF\xBF", 0x10FFFF},
		{"\xEF\xBF\xBE", 0xFFFE},
		{"\xEF\xBF\xBF", 0xFFFF},
		{"\x7F", 0x7F},
		{"\xC2\x80", 0x80},
		{"\xC3\xBF", 0xFF},
		{"\xC4\x80", 0x0100},
		{"\xDF\xBF", 0x7FF},
		{"\xE0\xA0\x80", 0x0800},
		{"\xE0\xBF\xBF", 0x0FFF},
		{"\xE1\x80\x80", 0x1000},
		{"\xF0\x90\x80\x80", 0x10000},
		{"\xF0\x91\x82\x80", 0x11080},
		{"\xF3\xA0\x80\x80", 0xE0000},
		{"\xF4\x8F\xBF\xBF", 0x10FFFF},
		{0, 0}
	};
	for (struct check *i = examples; i->src; ++i) {
		char dest[MB_LEN_MAX];
		size_t len = strlen(i->expected);
		CHECK(len == wctomb(dest, i->src));
		CHECK_STR(dest, i->expected, MB_LEN_MAX);
	}
}
TESTSUITE(wctomb_fail) {
	wchar_t examples[] = {
		0x001FFFFF,
		0x03FFFFFF,
		0x7FFFFFFF,
		0x00110000,
		0
	};
	for (wchar_t *i = examples; *i; ++i) {
		char dest[MB_LEN_MAX];
		CHECK((size_t)-1 == wctomb(dest, *i));
	}
}
#endif

