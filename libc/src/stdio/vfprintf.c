// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include "format.h"

int vfprintf(FILE *stream, const char *format, va_list arg)
{
	int written = 0;
	struct format_state state;
	_format_start(&state, format);
	while (!_format_done(&state) && !ferror(stream)) {
		struct format_chunk blob = _format_next(&state, &arg);
		written += fwrite(blob.addr, sizeof(char), blob.size, stream);
	}
	return ferror(stream)? EOF: written;
}


