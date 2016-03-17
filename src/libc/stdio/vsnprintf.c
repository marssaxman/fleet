// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <string.h>
#include "formatter.h"

int vsnprintf(char *s, size_t max, const char *format, va_list arg)
{
	int written = 0;
	struct format_state state;
	_format_start(&state, format);
	while (_format_next(&state, &arg)) {
		struct format_chunk blob = state.current;
		size_t copy = (max >= written + blob.size)? blob.size: (max - written);
		strncpy(&s[written], blob.addr, copy);
		written += copy;
		if (copy < blob.size) break;
	}
	return written;
}
