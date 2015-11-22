#include <stdio.h>
#include "format.h"

int vfprintf(FILE *stream, const char *format, va_list arg)
{
	int written = 0;
	struct format_state state;
	_format_start(&state, format);
	while (!_format_done(&state) && !ferror(stream)) {
		struct format_chunk blob = _format_next(&state, arg);
		written += fwrite(blob.addr, sizeof(char), blob.size, stream);
	}
	return ferror(stream)? EOF: written;
}


