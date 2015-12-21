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
	} else if (wc < 0x200000) {
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

