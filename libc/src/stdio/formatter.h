// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _LIBC_INTERNAL_FORMATTER_H
#define _LIBC_INTERNAL_FORMATTER_H

#include <stddef.h>
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
// We need a buffer large enough to contain any formatted number.
#define FORMAT_BUFFER_SIZE 32
struct format_state
{
	// Pointer to the next unread format string character
	const char *fmt;

	// Characters which have been produced by the current format specifier but
	// not yet returned from _format_next
	size_t leading_spaces;
	int prefix;
	size_t leading_zeros;
	struct format_chunk body;
	size_t trailing_zeros;
	bool trailing_point;
	size_t trailing_fraction;
	size_t trailing_spaces;

	// Temporary storage for specifiers other than %s
	char buffer[FORMAT_BUFFER_SIZE];
};

void _format_start(struct format_state*, const char *format_string);
bool _format_done(struct format_state*);
struct format_chunk _format_next(struct format_state*, va_list *arg);

#endif //_LIBC_INTERNAL_FORMATTER_H
