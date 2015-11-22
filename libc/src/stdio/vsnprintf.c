#include <stdio.h>
#include <string.h>
#include "format.h"

int vsnprintf(char *s, size_t max, const char *format, va_list arg)
{
	int written = 0;
	struct format_state state;
	_format_start(&state, format);
	while (!_format_done(&state)) {
		struct format_chunk blob = _format_next(&state, arg);
		size_t copy = (max >= written + blob.size)? blob.size: (max - written);
		strncpy(&s[written], blob.addr, copy);
		written += copy;
		if (copy < blob.size) break;
	}
	return written;
}
