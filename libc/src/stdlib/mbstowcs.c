// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>

size_t mbstowcs(wchar_t *dest, const char *src, size_t max)
{
	size_t total = 0;
	size_t read = 0;
	while(1) {
		int bytes = mbtowc(&dest[total], &src[read], max - read);
		// Check for decoding failure.
		if (bytes < 0) return (size_t)-1;
		// Check for string termination.
		if (bytes == 0) {
			dest[total] = 0;
			break;
		}
		++total;
		read += bytes;
	};
	return total;
}

#ifdef TESTSUITE
#include <wchar.h>
#include <string.h>
TESTSUITE(mbstowcs_match) {
	static wchar_t bogus1[] = {0x0080, 0};
	static wchar_t bogus2[] = {0x007F, 0};
	struct check {
		const wchar_t *expect;
		const char *src;
	} examples[] = {
		{L"A", "A"},
		{L"Humber", "Humber"},
		{bogus1, "\xC2\x80"},
		{L"\u00FF", "\xC3\xBF"},
		{L"\u0100", "\xC4\x80"},
		{L"\u07FF", "\xDF\xBF"},
		{L"\uD7FF", "\xED\x9F\xBF"},
		{L"\uE000", "\xEE\x80\x80"},
		{L"\uFFFD", "\xEF\xBF\xBD"},
		{L"\U0010FFFF", "\xF4\x8F\xBF\xBF"},
		{L"\uFFFE", "\xEF\xBF\xBE"},
		{L"\uFFFF", "\xEF\xBF\xBF"},
		{bogus2, "\x7F"},
		{L"\u0800", "\xE0\xA0\x80"},
		{L"\u0FFF", "\xE0\xBF\xBF"},
		{L"\u1000", "\xE1\x80\x80"},
		{L"\U00010000", "\xF0\x90\x80\x80"},
		{L"\U00011080", "\xF0\x91\x82\x80"},
		{L"\U000E0000", "\xF3\xA0\x80\x80"},
		{L"\U0010FFFF", "\xF4\x8F\xBF\xBF"},
		{L"H\u00E9ll\u00F6", "H\xC3\xA9ll\xC3\xB6"},
		{0, 0}
	};
	for (struct check *i = examples; i->expect; ++i) {
		wchar_t dest[16] = {0};
		size_t exlen = wcslen(i->expect);
		size_t srclen = strlen(i->src);
		CHECK(exlen == mbstowcs(dest, i->src, srclen+1));
		CHECK(exlen == mbstowcs(dest, i->src, 16));
		CHECK(0 == wcsncmp(dest, i->expect, 16));
		CHECK_MEM(dest, i->expect, exlen * sizeof(wchar_t));
	}
}
TESTSUITE(mbstowcs_fail) {
	struct check {
		const char *src;
	} examples[] = {
		{"\x80"},
		{"\xF3\x80"},
		{"\xF3\x80\x80"},
		{"\xBF"},
		{"\xE0 "},
		{"\xE1 "},
		{"\xE2 "},
		{"\xEF "},
		{"\xF0 "},
		{"\xF1 "},
		{"\xFA "},
		{"\xFB "},
		{"\xFC\x80\x80\x80\x80\x80"},
		{"\xFE"},
		{"\xFF"},
		{"\xFE\xFE\xFF\xFF"},
		{0}
	};
	const size_t fail = -1;
	for (struct check *i = examples; i->src; ++i) {
		wchar_t dest[16] = {0};
		size_t srclen = strlen(i->src);
		CHECK(fail == mbstowcs(dest, i->src, srclen));
		CHECK(fail == mbstowcs(dest, i->src, srclen+1));
	}
}
#endif //TESTSUITE
