// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>
#include <errno.h>

int mbtowc(wchar_t *dest, const char *src, size_t max)
{
	if (!src) {
		// This call is intended to reset our internal state, which we don't
		// have. We will return 0 to indicate that we have no shift sequences.
		return 0;
	}
	// Decode a UTF-8 codepoint and write it to the dest var, examining no more
	// than max bytes of input.
	char c = *src;
	if ('\0' == c) {
		// The input string is empty, therefore we convert no bytes.
		return 0;
	}
	// 0xxxxxxx: 7 bits
	// 110xxxxx 10xxxxxx: 11 bits
	// 1110xxxx 10xxxxxx 10xxxxxx: 16 bits
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx: 21 bits
	// UTF-8 can encode 26 and 32-bit sequences, but those are outside the
	// Unicode range, so we don't accept them.
	switch ((c >> 4) & 0x0F) {
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
			// 1-byte ascii
			if (max < 1) goto fail;
			*dest = c;
			return 1;
		case 0xC: case 0xD:
			// 2-byte sequence
			if (max < 2) goto fail;
			*dest = ((c & 0x1F) << 6) |
					(src[1] & 0x3F);
			return 2;
		case 0xE:
			// 3-byte sequence
			if (max < 3) goto fail;
			*dest = ((c & 0x0F) << 12) |
					((src[1] & 0x3F) << 6) |
					(src[2] & 0x3F);
			return 3;
		case 0xF:
			// probably a 4-byte sequence, but let's check
			if (c & 0x08) goto fail;
			if (max < 4) goto fail;
			*dest = ((c & 0x07) << 18) |
					((src[1] & 0x3F) << 12) |
					((src[2] & 0x3F) << 6) |
					(src[3] & 0x3F);
			return 4;
		default:
			// must be a continuing character: not legal
			goto fail;
	}
fail:
	errno = EILSEQ;
	return -1;
}

#ifdef TESTSUITE
#include <string.h>
TESTSUITE(mbtowc_match) {
	struct check {
		wchar_t expected;
		const char *src;
	} examples[] = {
		{0, ""},
		{L'a', "a"},
		{0xD7FF, "\xED\x9F\xBF"},
		{0xE000, "\xEE\x80\x80"},
		{0xFFFD, "\xEF\xBF\xBD"},
		{0x10FFFF, "\xF4\x8F\xBF\xBF"},
		{0xFFFE, "\xEF\xBF\xBE"},
		{0xFFFF, "\xEF\xBF\xBF"},
		{0x7F, "\x7F"},
		{0x80, "\xC2\x80"},
		{0xFF, "\xC3\xBF"},
		{0x0100, "\xC4\x80"},
		{0x7FF, "\xDF\xBF"},
		{0x0800, "\xE0\xA0\x80"},
		{0x0FFF, "\xE0\xBF\xBF"},
		{0x1000, "\xE1\x80\x80"},
		{0x10000, "\xF0\x90\x80\x80"},
		{0x11080, "\xF0\x91\x82\x80"},
		{0xE0000, "\xF3\xA0\x80\x80"},
		{0x10FFFF, "\xF4\x8F\xBF\xBF"},
		{0, 0}
	};
	for (struct check *i = examples; i->src; ++i) {
		wchar_t dest = 0;
		size_t len = strlen(i->src);
		CHECK(len == mbtowc(&dest, i->src, len));
		CHECK(dest == i->expected);
	}
}
TESTSUITE(mbtowc_fail) {
	struct check {
		const char *src;
	} examples[] = {
		"\x80",
		"\xF3\x80",
		"\xF3\x80\x80",
		"\xBF",
		"\xE0 ",
		"\xE1 ",
		"\xE2 ",
		"\xEF ",
		"\xF0 ",
		"\xF1 ",
		"\xFA ",
		"\xFB ",
		"\xFC\x80\x80\x80\x80\x80",
		"\xFE",
		"\xFF",
		"\xFE\xFE\xFF\xFF",
		0
	};
	for (struct check *i = examples; i->src; ++i) {
		wchar_t dest = 0;
		size_t len = strlen(i->src);
		CHECK(-1 == mbtowc(&dest, i->src, len));
	}
}
#endif

