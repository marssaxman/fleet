// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>

size_t mbstowcs(wchar_t *dest, const char *src, size_t max)
{
	size_t written = 0;
	while (max > 0) {
		int bytes = mbtowc(dest, src, max);
		// Check for decoding failure.
		if (bytes < 0) return (size_t)bytes;
		// Check for string termination.
		if (bytes == 0) {
			*dest = 0;
			break;
		} else {
			++written;
		}
		max -= bytes;
		src += bytes;
	};
	return written;
}

