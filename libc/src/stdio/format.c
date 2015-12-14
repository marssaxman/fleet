// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "format.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Not yet implemented:
// %ls - length specifier ignored, all strings are char*
// %lc - length specifier ignored, all chars are char
// precision for integer types (dioxX)
// floating-point types (fFeEgGaA)
// number-of-characters specifier(n) - unlikely to be supported ever
// pointer type (p)

#define MAX_PADDING 32
static const char padding[MAX_PADDING] = "                                ";
enum prefix {
	PREFIX_NONE = 0,
	PREFIX_PLUS = 1,
	PREFIX_MINUS = 2,
	PREFIX_SPACE = 3,
	PREFIX_OCTAL = 4,
	PREFIX_UPHEX = 5,
	PREFIX_LOWHEX = 6
};
static const struct format_chunk prefix_chunk[7] = {
	{"", 0}, // PREFIX_NONE
	{"+", 1}, // PREFIX_PLUS
	{"-", 1}, // PREFIX_MINUS
	{" ", 1}, // PREFIX_SPACE
	{"0", 1}, // PREFIX_OCTAL
	{"0X", 2}, // PREFIX_UPHEX
	{"0x", 2}, // PREFIX_LOWHEX
};

void _format_start(struct format_state *state, const char *format_string)
{
	memset(state, 0, sizeof(struct format_state));
	state->fmt = format_string;
}

bool _format_done(struct format_state *state)
{
	if (state->leading_spaces) return false;
	if (state->prefix) return false;
	if (state->body.size) return false;
	if (state->trailing_spaces) return false;
	return '\0' == *state->fmt;
}

static unsigned utoa(char *buf, uint64_t i, int radix, const char *digits)
{
	// Write the string in least-to-most significant order, then reverse it.
	unsigned len = 0;
	char *l = buf;
	do {
		*buf++ = digits[i % radix];
		i /= radix;
		len++;
	} while (i > 0);
	char *h = l + len - 1;
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
		case 1: return (int8_t)va_arg(arg, int);
		case 2: return (int16_t)va_arg(arg, int);
		case 4: return va_arg(arg, int32_t);
		case 8: return va_arg(arg, int64_t);
		default: return va_arg(arg, int);
	}
}

static uint64_t uarg(size_t s, va_list arg)
{
	switch (s) {
		case 1: return (uint8_t)va_arg(arg, unsigned);
		case 2: return (uint16_t)va_arg(arg, unsigned);
		case 4: return va_arg(arg, uint32_t);
		case 8: return va_arg(arg, uint64_t);
		default: return va_arg(arg, unsigned);
	}
}

static void scan_literal(struct format_state *state)
{
	state->body.addr = state->fmt;
	while (*state->fmt != '\0' && *state->fmt != '%') {
		state->fmt++;
	}
	state->body.size = state->fmt - state->body.addr;
}

static void parse_specifier(struct format_state *state, va_list arg)
{
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
		case '0': pad_with_zero = true; continue;
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
		fmt++;
	} else while (*fmt >= '0' && *fmt <= '9') {
		minimum_width = minimum_width * 10 + *fmt++ - '0';
	}

	// Precision begins with a '.' character if present; like the width, it
	// consists of a digit string or a '*' indicating that the value should
	// be read from the argument list.
	int precision = 0;
	bool has_precision = (*fmt == '.');
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
	state->body.addr = state->buffer;
	state->body.size = 0;
	switch (specifier) {
		case 'c': {
			state->buffer[0] = va_arg(arg, int);
			state->body.size = 1;
		} break;
		case 's': {
			const char *str = va_arg(arg, const char*);
			state->body.addr = str;
			if (str) {
				if (has_precision) {
					const char *q = memchr(str, '\0', precision);
					state->body.size = q? (q - str): precision;
				} else {
					state->body.size = strlen(str);
				}
			}
		} break;
		case 'i':
		case 'd': {
			int64_t num = iarg(length, arg);
			if (num < 0) {
				num = -num;
				state->prefix = PREFIX_MINUS;
			} else if (plus_for_positive) {
				state->prefix = PREFIX_PLUS;
			} else if (space_for_positive) {
				state->prefix = PREFIX_SPACE;
			}
			state->body.size = utoa(state->buffer, num, 10, digits_lower);
		} break;
		case 'x': {
			if (alternate_form) {
				state->prefix = PREFIX_LOWHEX;
			}
			uint64_t num = uarg(length, arg);
			state->body.size = utoa(state->buffer, num, 16, digits_lower);
		} break;
		case 'X': {
			if (alternate_form) {
				state->prefix = PREFIX_UPHEX;
			}
			uint64_t num = uarg(length, arg);
			state->body.size = utoa(state->buffer, num, 16, digits_upper);
		} break;
		case 'o': {
			if (alternate_form) {
				state->prefix = PREFIX_OCTAL;
			}
			uint64_t num = uarg(length, arg);
			state->body.size = utoa(state->buffer, num, 8, digits_lower);
		} break;
		case '%':
		default: {
			state->buffer[0] = specifier;
			state->body.size = 1;
		} break;
	}

	// If the text we've generated for this specifier is smaller than the
	// minimum field width, pad it out with spaces.
	if (minimum_width > state->body.size) {
		int padding = minimum_width - state->body.size;
		if (left_justify) {
			state->trailing_spaces = padding;
		} else {
			state->leading_spaces = padding;
		}
	}
}

struct format_chunk _format_next(struct format_state *state, va_list arg)
{
	struct format_chunk out = {padding, 0};
	while (!_format_done(state)) {
		if (state->leading_spaces > 0) {
			out.addr = padding;
			out.size = state->leading_spaces;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->leading_spaces -= out.size;
			break;
		}
		if (state->prefix) {
			out = prefix_chunk[state->prefix];
			state->prefix = PREFIX_NONE;
			break;
		}
		if (state->body.size > 0) {
			out = state->body;
			state->body.size = 0;
			break;
		}
		if (state->trailing_spaces > 0) {
			out.addr = padding;
			out.size = state->trailing_spaces;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->trailing_spaces -= out.size;
			break;
		}
		if ('%' == *state->fmt) {
			parse_specifier(state, arg);
		} else {
			scan_literal(state);
		}
	}
	return out;
}

#ifdef TESTSUITE
static char *testutoa(uint64_t n, int radix)
{
	static char buf[FORMAT_BUFFER_SIZE];
	buf[utoa(buf, n, radix, "0123456789ABCDEF")] = '\0';
	return buf;
}
static char *enhex(uint64_t n)
{
	return testutoa(n, 16);
}
static char *endec(uint64_t n)
{
	return testutoa(n, 10);
}
static char *enoct(uint64_t n)
{
	return testutoa(n, 8);
}
static char *enfmt(const char *fmt, ...)
{
	static struct format_state st;
	_format_start(&st, fmt);
	CHECK(!_format_done(&st));
	va_list arg;
	va_start(arg, fmt);
	static char buf[1024];
	char *dest = buf;
	while (!_format_done(&st)) {
		struct format_chunk chunk = _format_next(&st, arg);
		memcpy(dest, chunk.addr, chunk.size);
		dest += chunk.size;
	}
	va_end(arg);
	*dest = '\0';
	return buf;
}
TESTSUITE(format) {
	size_t size = FORMAT_BUFFER_SIZE;
	CHECK_STR(enoct(0), "0", size);
	CHECK_STR(enoct(1), "1", size);
	CHECK_STR(endec(0), "0", size);
	CHECK_STR(endec(1), "1", size);
	CHECK_STR(enhex(0), "0", size);
	CHECK_STR(enhex(1), "1", size);
	CHECK_STR(endec(100), "100", size);
	CHECK_STR(endec(127), "127", size);
	CHECK_STR(enhex(0x64), "64", size);
	CHECK_STR(enhex(0x7F), "7F", size);
	CHECK_STR(enhex(0x03E8), "3E8", size);
	CHECK_STR(endec(32749), "32749", size);
	CHECK_STR(enhex(0xFFE1), "FFE1", size);
	CHECK_STR(enhex(0xFFFF), "FFFF", size);
	CHECK_STR(endec(2305843009213693952LL), "2305843009213693952", size);
	CHECK_STR(enhex(0x7FFFFFFFFFFFFFFFULL), "7FFFFFFFFFFFFFFF", size);
	CHECK_STR(enhex(0xFFFFFFFFFFFFFFFFULL), "FFFFFFFFFFFFFFFF", size);
	CHECK_STR(endec(9999999999999999999ULL), "9999999999999999999", size);
	CHECK_STR(endec(9223372036854775807LL), "9223372036854775807", size);
	CHECK_STR(endec(18446744073709551615ULL), "18446744073709551615", size);
	CHECK_STR(enoct(01777777777777777777777), "1777777777777777777777", size);
	CHECK_STR(enfmt("%d", 1), "1", size);
	CHECK_STR(enfmt("%i", 0), "0", size);
	CHECK_STR(enfmt("%d", -1), "-1", size);
	CHECK_STR(enfmt("%x", 42), "2a", size);
	CHECK_STR(enfmt("%X", 42), "2A", size);
	CHECK_STR(enfmt("%#X", 42), "0X2A", size);
	CHECK_STR(enfmt("%#x", 42), "0x2a", size);
	CHECK_STR(enfmt("%#o", 0), "00", size);
	CHECK_STR(enfmt("%+i", 113976), "+113976", size);
	CHECK_STR(enfmt("%0i", 113976), "113976", size);
	CHECK_STR(enfmt("% i", 113976), " 113976", size);
	CHECK_STR(enfmt("% i", -113976), "-113976", size);
	CHECK_STR(enfmt("%s", "foo"), "foo", size);
	CHECK_STR(enfmt("%c", 'Q'), "Q", size);
	CHECK_STR(enfmt("%c", 0x41424344), "D", size);
	CHECK_STR(enfmt("%hhX", 0xFEDCBA9876543210), "10", size);
	CHECK_STR(enfmt("%hX", 0xFEDCBA9876543210), "3210", size);
	CHECK_STR(enfmt("%lX", 0xFEDCBA9876543210), "76543210", size);
	CHECK_STR(enfmt("%llX", 0xFEDCBA9876543210), "FEDCBA9876543210", size);
	CHECK_STR(enfmt("%Q", 0), "Q", size);
	CHECK_STR(enfmt("%-1q", 0), "q", size);
	CHECK_STR(enfmt("%%", 0), "%", size);
	CHECK_STR(enfmt("%Q", 0), "Q", size);
	CHECK_STR(enfmt("%8s", "foo"), "     foo", size);
	CHECK_STR(enfmt("%8s", "foobarbaz"), "foobarbaz", size);
	CHECK_STR(enfmt("%-8s", "foo"), "foo     ", size);
	CHECK_STR(enfmt("%-8s", "foobarbaz"), "foobarbaz", size);
	CHECK_STR(enfmt("X%8sX", ""), "X        X", size);
//	CHECK_STR(enfmt("%.7d", 12345), "0012345", size);
//	CHECK_STR(enfmt("%.7d", 123456789), "123456789", size);
//	CHECK_STR(enfmt("%#.6x", 0x1010), "0x001010", size);
//	CHECK_STR(enfmt("%.6d", -1099), "-001099", size);
}
#endif

