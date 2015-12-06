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

static unsigned utoa(char *buf, uint64_t i, int radix, const char *digits)
{
	// Write the string in least-to-most significant order, then reverse it.
	unsigned len = 0;
	do {
		*buf++ = digits[i % radix];
		i /= radix;
		len++;
	} while (i > 0);
	char *l = buf;
	char *h = &buf[len-1];
	while (l < h) {
		char temp = *h;
		*h-- = *l;
		*l++ = temp;
	}
	return len;
}

static int64_t iarg(size_t s, va_list arg)
{
	switch (s) {
		case 1: return va_arg(arg, int); //int8_t
		case 2: return va_arg(arg, int); //int16_t
		case 4: return va_arg(arg, int32_t);
		case 8: return va_arg(arg, int64_t);
		default: return va_arg(arg, int);
	}
}

static uint64_t uarg(size_t s, va_list arg)
{
	switch (s) {
		case 1: return va_arg(arg, unsigned); //uint8_t
		case 2: return va_arg(arg, unsigned); //uint16_t
		case 4: return va_arg(arg, uint32_t);
		case 8: return va_arg(arg, uint64_t);
		default: return va_arg(arg, unsigned);
	}
}

struct format_chunk _format_next(struct format_state *state, va_list arg)
{
	// Only call _format_next if you know there is more format string to read;
	// check this with !_format_done(state). We expect that state->fmt is not
	// null and that it contains at least one non-null character.

	// If the format string begins with literal text, scan up to the next
	// format specifier, or the end of the string, and return that chunk.
	if (*state->fmt != '%') {
		struct format_chunk out = {state->fmt, 0};
		while (*state->fmt != '\0' && *state->fmt != '%') {
			state->fmt++;
		}
		out.size = state->fmt - out.addr;
		if (out.size > 0) {
			return out;
		}
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

	static const char *digits_lower = "0123456789abcdef";
	static const char *digits_upper = "0123456789ABCDEF";

	// Having parsed all of the parameters, if present, the remaining character
	// is the format specifier.
	char specifier = *fmt;
	state->fmt = ++fmt;
	char *dest = state->buffer;
	switch (specifier) {
		case 'c': {
			*dest++ = va_arg(arg, int);
		} break;
		case 's': {
			struct format_chunk out = {va_arg(arg, const char*), 0};
			if (out.addr) {
				if (has_precision) {
					const char *q = memchr(out.addr, '\0', precision);
					out.size = q? (q - out.addr): precision;
				} else {
					out.size = strlen(out.addr);
				}
			}
			return out;
		}
		case 'i':
		case 'd': {
			int64_t num = iarg(length, arg);
			if (num < 0) {
				num = -num;
				*dest++ = '-';
			} else if (plus_for_positive) {
				*dest++ = '+';
			} else if (space_for_positive) {
				*dest++ = ' ';
			}
			dest += utoa(dest, num, 10, digits_lower);
		} break;
		case 'x': {
			if (alternate_form) {
				*dest++ = '0';
				*dest++ = 'x';
			}
			dest += utoa(dest, uarg(length, arg), 16, digits_lower);
		} break;
		case 'X': {
			if (alternate_form) {
				*dest++ = '0';
				*dest++ = 'X';
			}
			dest += utoa(dest, uarg(length, arg), 16, digits_upper);
		} break;
		case 'o': {
			if (alternate_form) {
				*dest++ = '0';
			}
			dest += utoa(dest, uarg(length, arg), 8, digits_lower);
		} break;
		case '%':
		default: {
			*dest++ = specifier;
		} break;
	}
	struct format_chunk out = {state->buffer, dest - state->buffer};
	return out;
}

#ifdef TESTSUITE
TESTSUITE(format) {

}
#endif

