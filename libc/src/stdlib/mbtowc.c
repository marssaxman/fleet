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
	switch (c >> 4) {
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

