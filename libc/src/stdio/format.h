#ifndef _LIBC_INTERNAL_FORMAT_H
#define _LIBC_INTERNAL_FORMAT_H

#include <_size_t.h>
#include <stdbool.h>
#include <stdarg.h>

// Each call to _format_next will produce a format_chunk, which can be written
// to the output stream or copied to the destination buffer as appropriate.
struct format_chunk
{
	const char *addr;
	size_t size;
};

// The formatter maintains its internal state in this record; don't alter it.
struct format_state
{
	const char *fmt;
	char buffer[1];
};

void _format_start(struct format_state*, const char *format_string);
bool _format_done(struct format_state*);
struct format_chunk _format_next(struct format_state*, va_list arg);

#endif //_LIBC_INTERNAL_FORMAT_H
