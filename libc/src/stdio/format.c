// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "format.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "fpconv.h"

// ERRATA:
// %s and %c ignore 'l' specifier; all strings are char*
// %a and %A are not yet implemented
// %n is not and will likely never be supported

#define MAX_PADDING 32
static const char padding[MAX_PADDING] = "                                ";
static const char zeros[MAX_PADDING] = "00000000000000000000000000000000";
static const char *digits_lower = "0123456789abcdef";
static const char *digits_upper = "0123456789ABCDEF";

enum prefix {
	PREFIX_NONE = 0,
	PREFIX_PLUS = 1,
	PREFIX_MINUS = 2,
	PREFIX_SPACE = 3,
	PREFIX_OCTAL = 4,
	PREFIX_UPHEX = 5,
	PREFIX_LOWHEX = 6,
	PREFIX_0DOT = 7,
	PREFIX_PLUS_0DOT = 8,
	PREFIX_MINUS_0DOT = 9,
	PREFIX_SPACE_0DOT = 10,
};

static const struct format_chunk prefix_chunk[11] = {
	{"", 0}, // PREFIX_NONE
	{"+", 1}, // PREFIX_PLUS
	{"-", 1}, // PREFIX_MINUS
	{" ", 1}, // PREFIX_SPACE
	{"0", 1}, // PREFIX_OCTAL
	{"0X", 2}, // PREFIX_UPHEX
	{"0x", 2}, // PREFIX_LOWHEX
	{"0.", 2}, // PREFIX_0DOT
	{"+0.", 3}, // PREFIX_PLUS_0DOT
	{"-0.", 3}, // PREFIX_MINUS_0DOT
	{" 0.", 3}, // PREFIX_SPACE_0DOT
};

enum flags {
	FLAG_LEFT_JUSTIFY = 0x01,
	FLAG_PLUS_POSITIVE = 0x02,
	FLAG_SPACE_POSITIVE = 0x04,
	FLAG_ALTERNATE_FORM = 0x08,
	FLAG_PAD_WITH_ZERO = 0x10,
	FLAG_HAS_PRECISION = 0x20,
	FLAG_UPPERCASE = 0x40,
	FLAG_TRIM_ZEROS = 0x80,
};

struct spec {
	int minimum_width;
	int precision;
	int data_size;
	char flags;
	char conversion;
};

static void scan_literal(struct format_state *state)
{
	state->body.addr = state->fmt;
	while (*state->fmt != '\0' && *state->fmt != '%') {
		state->fmt++;
	}
	state->body.size = state->fmt - state->body.addr;
}

static const char *parse(const char *fmt, va_list *arg, struct spec *spec)
{
	// A format specifier has this syntax:
	// %[flags][width][.precision][length]specifier
	// This function parses the body of the specifier, so 'fmt' should point
	// at the first character following the leading '%'.
	// We will return the address of the character following the specifier.

	// Flags may come in any combination and any order. Left-justification
	// can apply to any conversion, but '+' and ' ' only apply to float and
	// signed-integer conversions, '#' only applies to octal, hex, and float
	// conversions, and '0' only applies to float conversions and to integer
	// conversions which have no specified precision.
	spec->flags = 0;
	do switch (*fmt++) {
		case '-': spec->flags |= FLAG_LEFT_JUSTIFY; continue;
		case '+': spec->flags |= FLAG_PLUS_POSITIVE; continue;
		case ' ': spec->flags |= FLAG_SPACE_POSITIVE; continue;
		case '#': spec->flags |= FLAG_ALTERNATE_FORM; continue;
		case '0': spec->flags |= FLAG_PAD_WITH_ZERO; continue;
		default: --fmt;
	} while(0);

	// Width is a series of digits or a single '*' character indicating that
	// the value should come from the argument list.
	spec->minimum_width = 0;
	if ('*' == *fmt) {
		spec->minimum_width = va_arg(*arg, int);
		if (spec->minimum_width < 0) {
			spec->flags |= FLAG_LEFT_JUSTIFY;
			spec->minimum_width *= -1;
		}
		fmt++;
	} else while (*fmt >= '0' && *fmt <= '9') {
		spec->minimum_width = spec->minimum_width * 10 + *fmt++ - '0';
	}

	// Precision begins with a '.' character if present; like the width, it
	// consists of a digit string or a '*' indicating that the value should
	// be read from the argument list.
	spec->precision = 0;
	if ('.' == *fmt) {
		spec->flags |= FLAG_HAS_PRECISION;
		if ('*' == *++fmt) {
			spec->precision = va_arg(*arg, int);
			if (spec->precision < 0) {
				spec->precision = 0;
				spec->flags &= ~FLAG_HAS_PRECISION;
			}
			fmt++;
		} else while (*fmt >= '0' && *fmt <= '9') {
			spec->precision = spec->precision * 10 + *fmt++ - '0';
		}
	}

	// Size, which is called "length" in the spec, specifies the size of the
	// input data relative to sizeof(int). We assume that signed and unsigned
	// types have the same size.
	spec->data_size = sizeof(int);
	switch (*fmt++) {
		case 'h': {
			spec->data_size = sizeof(short);
			if ('h' == *fmt) {
				fmt++;
				spec->data_size = sizeof(char);
			}
		} break;
		case 'l': {
			spec->data_size = sizeof(long);
			if ('l' == *fmt) {
				fmt++;
				spec->data_size = sizeof(long long);
			}
		} break;
		case 'j': spec->data_size = sizeof(intmax_t); break;
		case 'z': spec->data_size = sizeof(size_t); break;
		case 't': spec->data_size = sizeof(ptrdiff_t); break;
		default: --fmt;
	}

	// The last remaining character is the conversion specifier.
	spec->conversion = *fmt++;

	if (spec->conversion >= 'A' && spec->conversion <= 'Z') {
		spec->flags |= FLAG_UPPERCASE;
	}

	return fmt;
}

static void cvt_c(struct format_state *state, struct spec *spec, va_list *arg)
{
	state->buffer[0] = va_arg(*arg, int);
	state->body.addr = state->buffer;
	state->body.size = 1;
}

static void cvt_s(struct format_state *state, struct spec *spec, va_list *arg)
{
	const char *str = va_arg(*arg, const char*);
	state->body.addr = str;
	if (str) {
		if (spec->flags & FLAG_HAS_PRECISION) {
			const char *q = memchr(str, '\0', spec->precision);
			state->body.size = q? (q - str): spec->precision;
		} else {
			state->body.size = strlen(str);
		}
	} else {
		state->body.size = 0;
	}
}

static void sign_prefix(
		struct format_state *state, const struct spec *spec, bool negative)
{
	if (negative) {
		state->prefix = PREFIX_MINUS;
	} else if (spec->flags & FLAG_PLUS_POSITIVE) {
		state->prefix = PREFIX_PLUS;
	} else if (spec->flags & FLAG_SPACE_POSITIVE) {
		state->prefix = PREFIX_SPACE;
	}
}

static void utoa(
		struct format_state *state,
		const struct spec *spec,
		uint64_t i,
		int radix,
		const char *digits)
{
	// Render the integer 'i' into the format_state buffer, then point the
	// state's 'body' chunk at the section of buffer we used. We will write
	// the number from the end of the buffer back toward the beginning, since
	// that gives us most-to-least significant digit order, and leaves the
	// beginning of the buffer free for padding and/or prefixes if necessary.
	char *buf = state->buffer + FORMAT_BUFFER_SIZE;
	bool has_precision = spec->flags & FLAG_HAS_PRECISION;
	if (i > 0 || !has_precision || spec->precision > 0) {
		do {
			*--buf = digits[i % radix];
			i /= radix;
		} while (i > 0);
	}
	state->body.addr = buf;
	state->body.size = FORMAT_BUFFER_SIZE - (buf - state->buffer);
	// If the spec requested a certain precision, make sure we have written
	// enough digits.
	if (has_precision && spec->precision > state->body.size) {
		state->leading_zeros = spec->precision - state->body.size;
	}
	// If the format includes a prefix, and there is room for it in the buffer,
	// copy it in so we don't have to return two separate chunks.
	if (state->prefix && 0 == spec->flags & FLAG_PAD_WITH_ZERO) {
		const struct format_chunk *prefix = &prefix_chunk[state->prefix];
		size_t combined_size = state->body.size + prefix->size;
		if (combined_size <= FORMAT_BUFFER_SIZE) {
			char *dest = state->buffer + FORMAT_BUFFER_SIZE - combined_size;
			memcpy(dest, prefix->addr, prefix->size);
			state->body.addr = dest;
			state->body.size = combined_size;
			state->prefix = PREFIX_NONE;
		}
	}
}

static int64_t iarg(size_t s, va_list *arg)
{
	switch (s) {
		case 1: return (int8_t)va_arg(*arg, int);
		case 2: return (int16_t)va_arg(*arg, int);
		case 4: return va_arg(*arg, int32_t);
		case 8: return va_arg(*arg, int64_t);
		default: return va_arg(*arg, int);
	}
}

static uint64_t uarg(size_t s, va_list *arg)
{
	switch (s) {
		case 1: return (uint8_t)va_arg(*arg, unsigned);
		case 2: return (uint16_t)va_arg(*arg, unsigned);
		case 4: return va_arg(*arg, uint32_t);
		case 8: return va_arg(*arg, uint64_t);
		default: return va_arg(*arg, unsigned);
	}
}

static void cvt_d(struct format_state *state, struct spec *spec, va_list *arg)
{
	int64_t num = iarg(spec->data_size, arg);
	sign_prefix(state, spec, num < 0);
	if (num < 0) {
		num = -num;
	}
	utoa(state, spec, num, 10, digits_lower);
}

static void cvt_x(struct format_state *state, struct spec *spec, va_list *arg)
{
	bool upper = spec->flags & FLAG_UPPERCASE;
	if (spec->flags & FLAG_ALTERNATE_FORM) {
		state->prefix = upper? PREFIX_UPHEX: PREFIX_LOWHEX;
	} else {
		state->prefix = PREFIX_NONE;
	}
	uint64_t num = uarg(spec->data_size, arg);
	utoa(state, spec, num, 16, upper? digits_upper: digits_lower);
}

static void cvt_o(struct format_state *state, struct spec *spec, va_list *arg)
{
	bool alternate_form = spec->flags & FLAG_ALTERNATE_FORM;
	state->prefix = alternate_form? PREFIX_OCTAL: PREFIX_NONE;
	uint64_t num = uarg(spec->data_size, arg);
	utoa(state, spec, num, 8, digits_lower);
}

static void cvt_p(struct format_state *state, struct spec *spec, va_list *arg)
{
	state->prefix = PREFIX_LOWHEX;
	uintptr_t num = va_arg(*arg, void*);
	utoa(state, spec, num, 16, digits_lower);
}

bool dtoa(struct format_state *state, struct spec *spec, double d, int *K)
{
	// Handle special values NAN, INF, and -INF, returning done (true).
	// Otherwise, use grisu2 to convert d into decimal, using the state
	// buffer. Pass the order value into parameter K and return false, since
	// the caller has more formatting work to do.
	union { double d; uint64_t u; } du;
	du.d = d;
	const uint64_t fracmask = 0x000FFFFFFFFFFFFFLLU;
	const uint64_t expmask = 0x7FF0000000000000LLU;
	const uint64_t hiddenbit = 0x0010000000000000LLU;
	const uint64_t signmask = 0x8000000000000000LLU;
	sign_prefix(state, spec, du.u & signmask);
	state->body.addr = state->buffer;
	bool done = true;
	if ((du.u & expmask) != expmask) {
		state->body.size = _fpconv_grisu2(d, state->buffer, K);
		done = false;
	} else if (du.u & fracmask != 0) {
		memcpy(state->buffer, spec->flags & FLAG_UPPERCASE? "NAN": "nan", 3);
		state->body.size = 3;
	} else {
		memcpy(state->buffer, spec->flags & FLAG_UPPERCASE? "INF": "inf", 3);
		state->body.size = 3;
	}
	return done;
}

static void adjust_precision(struct format_state *state, int off, int prec)
{
	int desired_len = prec + 1 + off;
	// If the value printed was too short, add some zeros until we build
	// up to the requested number of digits.
	while (state->body.size < desired_len) {
		state->buffer[state->body.size++] = '0';
	}
	// If the value is too long, round trailing digits until we have cut
	// the number down to the requested precision.
	while (state->body.size > desired_len) {
		char c = state->buffer[--state->body.size];
		if (c >= '5') {
			// round up
			int val = state->buffer[state->body.size - 1] - '0';
			val = (val < 9)? val + 1: val;
			state->buffer[state->body.size - 1] = val + '0';
		}
	}
}

static unsigned trim_zeros(const char *buf, unsigned size)
{
	while (size > 0 && '0' == buf[size-1]) {
		--size;
	}
	if (size > 0 && '.' == buf[size-1]) {
		--size;
	}
	return size;
}

static void emit_e(
		struct format_state *state, struct spec *spec, int K, int precision)
{
	int exp = K + state->body.size - 1;
	// For %e, precision controls the number of digits following the
	// decimal point, and alternate representation determines whether we
	// will print a decimal point if precision is < 2.
	adjust_precision(state, 0, precision);
	// Insert a decimal point after the first digit.
	if (precision > 0 || spec->flags & FLAG_ALTERNATE_FORM) {
		memmove(state->buffer + 2, state->buffer+1, precision);
		state->buffer[1] = '.';
		++state->body.size;
	}
	if (spec->flags & FLAG_TRIM_ZEROS) {
		state->body.size = trim_zeros(state->body.addr, state->body.size);
	}
	// Suffix with the exponent, which must always be at least 2 digits
	// and may never be more than 4 digits long. From the standard:
	// "The exponent always contains at least two digits, and only as many
	// more digits as necessary to represent the exponent."
	bool upper = spec->flags & FLAG_UPPERCASE;
	state->buffer[state->body.size++] = upper? 'E': 'e';
	state->buffer[state->body.size++] = (exp >= 0)? '+': '-';
	if (exp < 0) {
		exp = -exp;
	}
	if (exp >= 1000) {
		state->buffer[state->body.size++] = (exp / 1000) % 10 + '0';
	}
	if (exp >= 100) {
		state->buffer[state->body.size++] = (exp / 100) % 10 + '0';
	}
	state->buffer[state->body.size++] = (exp / 10) % 10 + '0';
	state->buffer[state->body.size++] = (exp / 1) % 10 + '0';
}

static void cvt_e(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int K = 0;
	if (dtoa(state, spec, num, &K)) return;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	emit_e(state, spec, K, precision);
}

static void emit_f(
		struct format_state *state, struct spec *spec, int K, int precision)
{
	// K represents the location of the decimal point relative to the end of
	// the string we just generated.
	if (K <= 0 && -K < state->body.size) {
		// The decimal point should be located somewhere within the string.
		// Move the characters, then either round or pad to make the number
		// of digits following the decimal equal to the precision requested.
		char *pos = state->buffer + state->body.size + K;
		memmove(pos + 1, pos, -K);
		*pos = '.';
		++state->body.size;
		adjust_precision(state, pos - state->buffer, precision);
		if (spec->flags & FLAG_TRIM_ZEROS) {
			state->body.size = trim_zeros(state->body.addr, state->body.size);
		}
	} else if (K <= 0) {
		// The number is smaller than 1, so it leads off with some "0."
		// prefix, depending on sign options.
		switch (state->prefix) {
			case PREFIX_NONE: state->prefix = PREFIX_0DOT; break;
			case PREFIX_MINUS: state->prefix = PREFIX_MINUS_0DOT; break;
			case PREFIX_PLUS: state->prefix = PREFIX_PLUS_0DOT; break;
			case PREFIX_SPACE: state->prefix = PREFIX_SPACE_0DOT; break;
			default: state->prefix = PREFIX_0DOT;
		}
		// How many leading zeros do we need?
		state->leading_zeros = -(K + state->body.size);
		if (state->leading_zeros < precision) {
			// If there's room for more digits, use the ones we generated.
			int available = precision - state->leading_zeros;
			if (state->body.size > available) {
				state->body.size = available;
			}
		} else {
			// Don't generate more zeros than we actually need to show.
			state->leading_zeros = precision;
			state->body.size = 0;
		}
		if (spec->flags & FLAG_TRIM_ZEROS) {
			state->body.size = trim_zeros(state->body.addr, state->body.size);
		}
	} else {
		// The decimal point appears somewhere to the right of the string.
		// This implies that all the fractional digits are also zeros.
		state->trailing_zeros = K;
		if (0 == (spec->flags & FLAG_TRIM_ZEROS)) {
			state->trailing_point = true;
			state->trailing_fraction = precision;
		}
	}
}

static void cvt_f(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int K = 0;
	if (dtoa(state, spec, num, &K)) return;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	emit_f(state, spec, K, precision);
}

static void cvt_g(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int K = 0;
	if (dtoa(state, spec, num, &K)) return;
    int exp = K + state->body.size - 1;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	if (precision < 1) {
		precision = 1;
	}
	// With mode %g, precision controls the total number of significant digits,
	// not (as with %e and %f) the number of digits after the decimal point.
	// We'll print very large or small numbers using scientific notation, while
	// numbers closer to zero will be printed as simple decimals.
	spec->flags |= FLAG_TRIM_ZEROS;
	if (exp < -4 || exp >= precision) {
		emit_e(state, spec, K, precision - 1);
	} else {
		if (exp >= 0) precision -= (exp+1);
		emit_f(state, spec, K, precision);
	}
}

static void convert(struct format_state *state, va_list *arg)
{
	// Skip the leading % character and parse the specifier body.
	struct spec spec = {0};
	state->fmt = parse(++state->fmt, arg, &spec);

	// Zero-padding only applies to right-justified conversions; if the '-'
	// flag was preset, we'll ignore '0', whichever order they occurred in.
	if (spec.flags & FLAG_LEFT_JUSTIFY) {
		spec.flags &= ~FLAG_PAD_WITH_ZERO;
	}
	// Using a space for positive numbers only applies if we are not already
	// using a plus sign; if the '+' flag was present, ignore ' '.
	if (spec.flags & FLAG_PLUS_POSITIVE) {
		spec.flags &= ~FLAG_SPACE_POSITIVE;
	}

	switch (spec.conversion) {
		case 'c': cvt_c(state, &spec, arg); break;
		case 's': cvt_s(state, &spec, arg); break;
		case 'i':
		case 'd': cvt_d(state, &spec, arg); break;
		case 'x':
		case 'X': cvt_x(state, &spec, arg); break;
		case 'o': cvt_o(state, &spec, arg); break;
		case 'p': cvt_p(state, &spec, arg); break;
		case 'g':
		case 'G': cvt_g(state, &spec, arg); break;
		case 'e':
		case 'E': cvt_e(state, &spec, arg); break;
		case 'f':
		case 'F': cvt_f(state, &spec, arg); break;
		case '%':
		default: {
			state->buffer[0] = spec.conversion;
			state->body.addr = state->buffer;
			state->body.size = 1;
		} break;
	}

	// Combining the three possible body elements, how many characters did
	// we just generate?
	size_t width = 0;
	width += state->leading_spaces;
	width += prefix_chunk[state->prefix].size;
	width += state->leading_zeros;
	width += state->body.size;
	width += state->trailing_spaces;

	// If the text we've generated for this specifier is smaller than the
	// minimum field width, pad it out with spaces.
	int minimum_width = spec.minimum_width;
	if (minimum_width > width) {
		int padding = minimum_width - width;
		bool left_justify = spec.flags & FLAG_LEFT_JUSTIFY;
		bool pad_with_zero = spec.flags & FLAG_PAD_WITH_ZERO;
		if (left_justify) {
			state->trailing_spaces += padding;
		} else if (pad_with_zero) {
			state->leading_zeros += padding;
		} else {
			state->leading_spaces += padding;
		}
	}
}

void _format_start(struct format_state *state, const char *format_string)
{
	memset(state, 0, sizeof(struct format_state));
	state->fmt = format_string;
}

bool _format_done(struct format_state *state)
{
	if (state->leading_spaces) return false;
	if (state->prefix) return false;
	if (state->leading_zeros) return false;
	if (state->body.size) return false;
	if (state->trailing_zeros) return false;
	if (state->trailing_point) return false;
	if (state->trailing_fraction) return false;
	if (state->trailing_spaces) return false;
	return '\0' == *state->fmt;
}

struct format_chunk _format_next(struct format_state *state, va_list *arg)
{
	struct format_chunk out = {0, 0};
	while (!_format_done(state)) {
		if (state->leading_spaces) {
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
		if (state->leading_zeros) {
			out.addr = zeros;
			out.size = state->leading_zeros;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->leading_zeros -= out.size;
			break;
		}
		if (state->body.size) {
			out = state->body;
			state->body.size = 0;
			break;
		}
		if (state->trailing_zeros) {
			out.addr = zeros;
			out.size = state->trailing_zeros;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->trailing_zeros -= out.size;
			break;
		}
		if (state->trailing_point) {
			out.addr = ".";
			out.size = 1;
			state->trailing_point = false;
			break;
		}
		if (state->trailing_fraction) {
			out.addr = zeros;
			out.size = state->trailing_fraction;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->trailing_fraction -= out.size;
			break;
		}
		if (state->trailing_spaces) {
			out.addr = padding;
			out.size = state->trailing_spaces;
			if (out.size > MAX_PADDING) out.size = MAX_PADDING;
			state->trailing_spaces -= out.size;
			break;
		}
		if ('%' == *state->fmt) {
			convert(state, arg);
		} else {
			scan_literal(state);
		}
	}
	return out;
}

#ifdef TESTSUITE
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
		struct format_chunk chunk = _format_next(&st, &arg);
		memcpy(dest, chunk.addr, chunk.size);
		dest += chunk.size;
	}
	va_end(arg);
	*dest = '\0';
	return buf;
}
TESTSUITE(format) {
	size_t size = FORMAT_BUFFER_SIZE;
	CHECK_STR(enfmt("%o", 0), "0", size);
	CHECK_STR(enfmt("%o", 1), "1", size);
	CHECK_STR(enfmt("%d", 0), "0", size);
	CHECK_STR(enfmt("%d", 1), "1", size);
	CHECK_STR(enfmt("%X", 0), "0", size);
	CHECK_STR(enfmt("%X", 1), "1", size);
	CHECK_STR(enfmt("%d", 100), "100", size);
	CHECK_STR(enfmt("%d", 127), "127", size);
	CHECK_STR(enfmt("%X", 0x64), "64", size);
	CHECK_STR(enfmt("%X", 0x7F), "7F", size);
	CHECK_STR(enfmt("%X", 0x03E8), "3E8", size);
	CHECK_STR(enfmt("%d", 32749), "32749", size);
	CHECK_STR(enfmt("%X", 0xFFE1), "FFE1", size);
	CHECK_STR(enfmt("%X", 0xFFFF), "FFFF", size);
	CHECK_STR(enfmt("%lld", 2305843009213693952LL), "2305843009213693952", size);
	CHECK_STR(enfmt("%llX", 0x7FFFFFFFFFFFFFFFULL), "7FFFFFFFFFFFFFFF", size);
	CHECK_STR(enfmt("%llX", 0xFFFFFFFFFFFFFFFFULL), "FFFFFFFFFFFFFFFF", size);
	CHECK_STR(enfmt("%lld", 999999999999999999ULL), "999999999999999999", size);
	CHECK_STR(enfmt("%lld", 9223372036854775807LL), "9223372036854775807", size);
	CHECK_STR(enfmt("%lld", 8446744073709551615ULL), "8446744073709551615", size);
	CHECK_STR(enfmt("%llo", 0xFFFFFFFFFFFFFFFF), "1777777777777777777777", size);
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
	CHECK_STR(enfmt("%.5s", "foobaramatimatus"), "fooba", size);
	CHECK_STR(enfmt("%9.5s", "foobaramatimatus"), "    fooba", size);
	CHECK_STR(enfmt("%5.9s", "foo"), "  foo", size);
	CHECK_STR(enfmt("%5.9s", "foobaramatimatus"), "foobarama", size);
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
	CHECK_STR(enfmt("%+8d", -12345), "  -12345", size);
	CHECK_STR(enfmt("%8d", -12345), "  -12345", size);
	CHECK_STR(enfmt("%+8d", 12345), "  +12345", size);
	CHECK_STR(enfmt("%8d", 12345), "   12345", size);
	CHECK_STR(enfmt("%.7d", 12345), "0012345", size);
	CHECK_STR(enfmt("%.7d", 123456789), "123456789", size);
	CHECK_STR(enfmt("%#.6x", 0x1010), "0x001010", size);
	CHECK_STR(enfmt("%.6d", -1099), "-001099", size);
	CHECK_STR(enfmt("%*d", 6, 4909), "  4909", size);
	CHECK_STR(enfmt("%*d:%0*d", 6, 4909, 6, 101), "  4909:000101", size);
	CHECK_STR(enfmt("%04d %05d %06d", 1, 2, 3), "0001 00002 000003", size);
	CHECK_STR(enfmt("%03d %04d %05d", -1, -2, -3), "-01 -002 -0003", size);
	CHECK_STR(enfmt("%.3d %.4d %.5d", -1, -2, -3), "-001 -0002 -00003", size);
	CHECK_STR(enfmt("%06.5d", 5), "000005", size);
	CHECK_STR(enfmt("%06.5d", -5), "-00005", size);
	CHECK_STR(enfmt("%6.4d", 5), "  0005", size);
	CHECK_STR(enfmt("%6.4d", -5), " -0005", size);
	CHECK_STR(enfmt("%p %p", (void*)99, (void*)365), "0x63 0x16d", size);
	CHECK_STR(enfmt("%40d", 99), "                                      99", size);
	CHECK_STR(enfmt("%040d", 99), "0000000000000000000000000000000000000099", size);
	CHECK_STR(enfmt("%40d", -99), "                                      -99", size);
	CHECK_STR(enfmt("%040d", -99), "-0000000000000000000000000000000000000099", size);
	CHECK_STR(enfmt("%.40d", 99), "0000000000000000000000000000000000000099", size);
	CHECK_STR(enfmt("%.40d", -99), "-0000000000000000000000000000000000000099", size);
	CHECK_STR(enfmt("X%.0dX", 0), "XX", size);
	CHECK_STR(enfmt("X%.*dX", 0, 0), "XX", size);
	CHECK_STR(enfmt("X%.*dX", -2, 0), "X0X", size);
	CHECK_STR(enfmt("%g", 1.5), "1.5", size);
	CHECK_STR(enfmt("%g", 10000.0), "10000", size);
	CHECK_STR(enfmt("%g", 1010.9932), "1010.99", size);
	CHECK_STR(enfmt("%.8g", 1010.9932), "1010.9932", size);
	CHECK_STR(enfmt("%g", 1.0 / 3.0), "0.333333", size);
#if 0
	union hackfloat {
		float f;
		uint32_t u;
	} inf;
	inf.u = 0x7F800000;
	CHECK_STR(enfmt("%g", inf.f), "inf", size);
	CHECK_STR(enfmt("%G", inf.f), "INF", size);
	CHECK_STR(enfmt("%g", -inf.f), "-inf", size);
	CHECK_STR(enfmt("%G", -inf.f), "-INF", size);
	union hackfloat nan;
	nan.u = 0x7FFFFFFF;
	CHECK_STR(enfmt("%g", nan.f), "nan", size);
	CHECK_STR(enfmt("%G", nan.f), "NAN", size);
	CHECK_STR(enfmt("%g", -nan.f), "-nan", size);
	CHECK_STR(enfmt("%G", -nan.f), "-NAN", size);
	CHECK_STR(enfmt("%e", 100.0), "1.000000e+02", size);
	CHECK_STR(enfmt("%e", 1.5), "1.500000e+00", size);
	CHECK_STR(enfmt("%e", 10000.0), "1.000000e+04", size);
	CHECK_STR(enfmt("%e", 1010.9932), "1.010993e+03", size);
	CHECK_STR(enfmt("%e", 1.0 / 3.0), "3.333333e-01", size);
	CHECK_STR(enfmt("%12.4e", 1.0 / 3.0), "  3.3333e-01", size);
	CHECK_STR(enfmt("%E", 100.0), "1.000000E+02", size);
	CHECK_STR(enfmt("%E", 1.5), "1.500000E+00", size);
	CHECK_STR(enfmt("%.0E", 10000.0), "1E+04", size);
	CHECK_STR(enfmt("%E", 1010.9932), "1.010993E+03", size);
	CHECK_STR(enfmt("%E", 1.0 / 3.0), "3.333333E-01", size);
	CHECK_STR(enfmt("%12.4E", 1.0 / 3.0), "  3.3333E-01", size);
#endif
	// 3.666482e-13
	CHECK_STR(enfmt("f %f", 0x3d59ccf240000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3d59ccf240000000), "e 3.666482e-13", size);
	CHECK_STR(enfmt("g %g", 0x3d59ccf240000000), "g 3.66648e-13", size);
	// 2.083897e-35
	CHECK_STR(enfmt("f %f", 0x38bbb32240000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x38bbb32240000000), "e 2.083897e-35", size);
	CHECK_STR(enfmt("g %g", 0x38bbb32240000000), "g 2.0839e-35", size);
	// 2.058279e-16
	CHECK_STR(enfmt("f %f", 0x3cada9b5c0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3cada9b5c0000000), "e 2.058279e-16", size);
	CHECK_STR(enfmt("g %g", 0x3cada9b5c0000000), "g 2.05828e-16", size);
	// 3.190765e+37 - %f differs from glibc here
	CHECK_STR(enfmt("f %f", 0x47b8013060000000), "f 31907645357261092000000000000000000000.000000", size);
	CHECK_STR(enfmt("e %e", 0x47b8013060000000), "e 3.190765e+37", size);
	CHECK_STR(enfmt("g %g", 0x47b8013060000000), "g 3.19076e+37", size);
	// 3.269213e+06
	CHECK_STR(enfmt("f %f", 0x4148f12e60000000), "f 3269212.750000", size);
	CHECK_STR(enfmt("e %e", 0x4148f12e60000000), "e 3.269213e+06", size);
	CHECK_STR(enfmt("g %g", 0x4148f12e60000000), "g 3.26921e+06", size);
	// 7.271168e+01
	CHECK_STR(enfmt("f %f", 0x40522d8c20000000), "f 72.711678", size);
	CHECK_STR(enfmt("e %e", 0x40522d8c20000000), "e 7.271168e+01", size);
	CHECK_STR(enfmt("g %g", 0x40522d8c20000000), "g 72.7117", size);
	// 1.036129e-35
	CHECK_STR(enfmt("f %f", 0x38ab8b86c0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x38ab8b86c0000000), "e 1.036129e-35", size);
	CHECK_STR(enfmt("g %g", 0x38ab8b86c0000000), "g 1.03613e-35", size);
	// 7.454966e+14
	CHECK_STR(enfmt("f %f", 0x43053033a0000000), "f 745496599592960.000000", size);
	CHECK_STR(enfmt("e %e", 0x43053033a0000000), "e 7.454966e+14", size);
	CHECK_STR(enfmt("g %g", 0x43053033a0000000), "g 7.45497e+14", size);
	// 1.131185e+21 - %f differs from glibc here
	CHECK_STR(enfmt("f %f", 0x444ea92c40000000), "f 1131185021742831600000.000000", size);
	CHECK_STR(enfmt("e %e", 0x444ea92c40000000), "e 1.131185e+21", size);
	CHECK_STR(enfmt("g %g", 0x444ea92c40000000), "g 1.13119e+21", size);
	// 9.210065e-11
	CHECK_STR(enfmt("f %f", 0x3dd95101c0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3dd95101c0000000), "e 9.210065e-11", size);
	CHECK_STR(enfmt("g %g", 0x3dd95101c0000000), "g 9.21006e-11", size);
	// 2.112093e-13
	CHECK_STR(enfmt("f %f", 0x3d4db99e40000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3d4db99e40000000), "e 2.112093e-13", size);
	CHECK_STR(enfmt("g %g", 0x3d4db99e40000000), "g 2.11209e-13", size);
	// 5.350548e+00
	CHECK_STR(enfmt("f %f", 0x401566f600000000), "f 5.350548", size);
	CHECK_STR(enfmt("e %e", 0x401566f600000000), "e 5.350548e+00", size);
	CHECK_STR(enfmt("g %g", 0x401566f600000000), "g 5.35055", size);
	// 2.837902e-27
	CHECK_STR(enfmt("f %f", 0x3a6c1aefe0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3a6c1aefe0000000), "e 2.837902e-27", size);
	CHECK_STR(enfmt("g %g", 0x3a6c1aefe0000000), "g 2.8379e-27", size);
	// 3.311628e-39
	CHECK_STR(enfmt("f %f", 0x37f207bc80000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x37f207bc80000000), "e 3.311628e-39", size);
	CHECK_STR(enfmt("g %g", 0x37f207bc80000000), "g 3.31163e-39", size);
	// 1.420224e+13
	CHECK_STR(enfmt("f %f", 0x42a9d56e60000000), "f 14202235256832.000000", size);
	CHECK_STR(enfmt("e %e", 0x42a9d56e60000000), "e 1.420224e+13", size);
	CHECK_STR(enfmt("g %g", 0x42a9d56e60000000), "g 1.42022e+13", size);
	// 8.922247e+00
	CHECK_STR(enfmt("f %f", 0x4021d830c0000000), "f 8.922247", size);
	CHECK_STR(enfmt("e %e", 0x4021d830c0000000), "e 8.922247e+00", size);
	CHECK_STR(enfmt("g %g", 0x4021d830c0000000), "g 8.92225", size);
	// 5.889534e+20 - %f differs from glibc here
	CHECK_STR(enfmt("f %f", 0x443fed5ee0000000), "f 588953421010400440000.000000", size);
	CHECK_STR(enfmt("e %e", 0x443fed5ee0000000), "e 5.889534e+20", size);
	CHECK_STR(enfmt("g %g", 0x443fed5ee0000000), "g 5.88953e+20", size);
	// 1.207875e+29 - %f differs from glibc here
	CHECK_STR(enfmt("f %f", 0x45f8649160000000), "f 120787509307219070000000000000.000000", size);
	CHECK_STR(enfmt("e %e", 0x45f8649160000000), "e 1.207875e+29", size);
	CHECK_STR(enfmt("g %g", 0x45f8649160000000), "g 1.20788e+29", size);
	// 7.604723e-38
	CHECK_STR(enfmt("f %f", 0x3839e0a5e0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3839e0a5e0000000), "e 7.604723e-38", size);
	CHECK_STR(enfmt("g %g", 0x3839e0a5e0000000), "g 7.60472e-38", size);
	// 8.140378e-30
	CHECK_STR(enfmt("f %f", 0x39e4a36840000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x39e4a36840000000), "e 8.140378e-30", size);
	CHECK_STR(enfmt("g %g", 0x39e4a36840000000), "g 8.14038e-30", size);
	// 2.872209e-13
	CHECK_STR(enfmt("f %f", 0x3d54361c80000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3d54361c80000000), "e 2.872209e-13", size);
	CHECK_STR(enfmt("g %g", 0x3d54361c80000000), "g 2.87221e-13", size);
	// 1.760535e-09
	CHECK_STR(enfmt("f %f", 0x3e1e3eeaa0000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3e1e3eeaa0000000), "e 1.760535e-09", size);
	CHECK_STR(enfmt("g %g", 0x3e1e3eeaa0000000), "g 1.76054e-09", size);
	// 5.901336e-18
	CHECK_STR(enfmt("f %f", 0x3c5b371140000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3c5b371140000000), "e 5.901336e-18", size);
	CHECK_STR(enfmt("g %g", 0x3c5b371140000000), "g 5.90134e-18", size);
	// 1.809661e-26
	CHECK_STR(enfmt("f %f", 0x3a96670b20000000), "f 0.000000", size);
	CHECK_STR(enfmt("e %e", 0x3a96670b20000000), "e 1.809661e-26", size);
	CHECK_STR(enfmt("g %g", 0x3a96670b20000000), "g 1.80966e-26", size);
}
#endif

