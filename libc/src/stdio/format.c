// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "format.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void _format_start(struct format_state *state, const char *format_string)
{
	state->fmt = format_string;
}

bool _format_done(struct format_state *state)
{
	return '\0' == *state->fmt;
}

struct format_chunk _format_next(struct format_state *state, va_list arg)
{
	// Only call _format_next if you know there is more format string to read;
	// check this with !_format_done(state). We expect that state->fmt is not
	// null and that it contains at least one non-null character.

	// If the format string begins with literal text, scan up to the next
	// format specifier, or the end of the string, and return that chunk.
	struct format_chunk out = {state->fmt, 0};
	while (*state->fmt != '\0' && *state->fmt != '%') {
		state->fmt++;
	}
	out.size = state->fmt - out.addr;
	if (out.size > 0) {
		return out;
	}

	// We did not find a literal text chunk, so we must have found a specifier.
	// A format specifier has this syntax:
	// %[flags][width][.precision][length]specifier
	// Skip the leading % character.
	const char *fmt = ++state->fmt;

	// Flags may come in any combination and any order.
	bool left_justify = false;
	bool plus_for_positive = false;
	bool space_for_positive = false;
	bool alternate_form = false;
	bool pad_with_zero = false;
	do switch (*fmt++) {
		case '-': left_justify = true; continue;
		case '+': plus_for_positive = true; continue;
		case ' ': space_for_positive = true; continue;
		case '#': alternate_form = true; continue;
		case '0': left_justify = true; continue;
		default: --fmt;
	} while(0);

	// Width is a series of digits or a single '*' character indicating that
	// the value should come from the argument list.
	int minimum_width = 0;
	if ('*' == *fmt) {
		minimum_width = va_arg(arg, int);
		if (minimum_width < 0) {
			left_justify = true;
			minimum_width *= -1;
		}
	} else while ('0' >= *fmt && *fmt <= '9') {
		minimum_width = minimum_width * 10 + *fmt++ - '0';
	}

	// Precision begins with a '.' character if present; like the width, it
	// consists of a digit string or a '*' indicating that the value should
	// be read from the argument list.
	int precision = 0;
	bool has_precision = *fmt == '.';
	if (has_precision) {
		if ('*' == *++fmt) {
			precision = va_arg(arg, int);
			if (precision < 0) {
				precision = 0;
				has_precision = false;
			}
		} else while (*fmt >= '0' && *fmt <= '9') {
			precision = precision * 10 + *fmt++ - '0';
		}
	}

	// We assume that signed and unsigned types have the same size.
	int length = sizeof(int);
	switch (*fmt++) {
		case 'h': {
			length = sizeof(short);
			if ('h' == *fmt) {
				fmt++;
				length = sizeof(char);
			}
		} break;
		case 'l': {
			length = sizeof(long);
			if ('l' == *fmt) {
				fmt++;
				length = sizeof(long long);
			}
		} break;
		case 'j': length = sizeof(intmax_t); break;
		case 'z': length = sizeof(size_t); break;
		case 't': length = sizeof(ptrdiff_t); break;
		default: --fmt;
	}

	// Having parsed all of the parameters, if present, the remaining character
	// is the format specifier.
	char specifier = *fmt;
	state->fmt = ++fmt;
	switch (specifier) {
		case '%': {
			state->buffer[0] = '%';
			out.addr = state->buffer;
			out.size = 1;
		} break;
		case 'c': {
			state->buffer[0] = va_arg(arg, int);
			out.addr = state->buffer;
			out.size = 1;
		} break;
		case 's': {
			out.addr = va_arg(arg, const char*);
			if (out.addr) {
				if (has_precision) {
					const char *q = memchr(out.addr, '\0', precision);
					out.size = q? (q - out.addr): precision;
				} else {
					out.size = strlen(out.addr);
				}
			}
		} break;
	}
	return out;
}


