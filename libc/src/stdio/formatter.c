// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "formatter.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "fpconv.h"

// ERRATA:
// %s and %c ignore 'l' specifier; all strings are char*
// %n is not and will likely never be supported

#define MAX_PADDING 32
static const char padding[MAX_PADDING] = "                                ";
static const char zeros[MAX_PADDING] = "00000000000000000000000000000000";

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

static uint64_t ulabs(int64_t n) { return (n < 0)? -n: n; }
static double dabs(double n) { return (n < 0.0)? -n: n; }

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
		int radix)
{
	// Render the integer 'i' into the format_state buffer, then point the
	// state's 'body' chunk at the section of buffer we used. We will write
	// the number from the end of the buffer back toward the beginning, since
	// that gives us most-to-least significant digit order, and leaves the
	// beginning of the buffer free for padding and/or prefixes if necessary.
	bool upper = spec->flags & FLAG_UPPERCASE;
	const char *digits = upper? "0123456789ABCDEF": "0123456789abcdef";
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
	utoa(state, spec, ulabs(num), 10);
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
	utoa(state, spec, num, 16);
}

static void cvt_o(struct format_state *state, struct spec *spec, va_list *arg)
{
	bool alternate_form = spec->flags & FLAG_ALTERNATE_FORM;
	state->prefix = alternate_form? PREFIX_OCTAL: PREFIX_NONE;
	uint64_t num = uarg(spec->data_size, arg);
	utoa(state, spec, num, 8);
}

static void cvt_p(struct format_state *state, struct spec *spec, va_list *arg)
{
	state->prefix = PREFIX_LOWHEX;
	uintptr_t num = va_arg(*arg, void*);
	utoa(state, spec, num, 16);
}

bool dtoa(struct format_state *state, struct spec *spec, double d, int *exp)
{
	// Handle special values NAN, INF, and -INF, returning done (true).
	// Otherwise, use grisu2 to convert d into decimal, using the state
	// buffer. Pass the order value into parameter K and return false, since
	// the caller has more formatting work to do.
	union { double d; uint64_t u; } du;
	du.d = d;
	const uint64_t fracmask =  0x000FFFFFFFFFFFFFLLU;
	const uint64_t expmask =   0x7FF0000000000000LLU;
	const uint64_t signmask =  0x8000000000000000LLU;
	sign_prefix(state, spec, du.u & signmask);
	state->body.addr = state->buffer;
	bool done = true;
	if ((du.u & expmask) != expmask) {
		int K = 0;
		state->body.size = _fpconv_grisu2(d, state->buffer, &K);
		*exp = K + state->body.size - 1;
		done = false;
	} else {
		const char *msg = (du.u & fracmask)? "nanNAN": "infINF";
		if (spec->flags & FLAG_UPPERCASE) {
			msg += 3;
		}
		memcpy(state->buffer, msg, 3);
		state->body.size = 3;
	}
	return done;
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

static bool set_sigfigs(struct format_state *state, int new_size)
{
	bool shift = false;
	char *buf = state->buffer;
	int old_size = state->body.size;
	int diff = new_size - old_size;
	if (diff > 0) {
		memset(buf + old_size, '0', diff);
	}
	if (diff < 0) {
		char c = buf[new_size];
		bool carry = (c >= '5' && c <= '9');
		int index = new_size;
		while (carry) {
			if (index > 0) {
				--index;
			} else {
				memmove(buf+1, buf, new_size-1);
				*buf = '0';
				shift = true;
			}
			c = buf[index];
			carry = (c == '9');
			buf[index] = carry? '0': c+1;
		}
	}
	state->body.size = new_size;
	return shift;
}

static void emit_e(struct format_state *state, struct spec *spec, int exp)
{
	char *buf = state->buffer;
	int size = state->body.size;
	// Insert a decimal point after the first digit.
	if (size > 1 || spec->flags & FLAG_ALTERNATE_FORM) {
		memmove(buf + 2, buf + 1, size - 1);
		buf[1] = '.';
		++size;
	}
	if (spec->flags & FLAG_TRIM_ZEROS) {
		size = trim_zeros(buf, size);
	}
	// Suffix with the exponent, which must always be at least 2 digits
	// and may never be more than 4 digits long. From the standard:
	// "The exponent always contains at least two digits, and only as many
	// more digits as necessary to represent the exponent."
	bool upper = spec->flags & FLAG_UPPERCASE;
	buf[size++] = upper? 'E': 'e';
	buf[size++] = (exp >= 0)? '+': '-';
	exp = ulabs(exp);
	if (exp >= 1000) {
		buf[size++] = (exp / 1000) % 10 + '0';
	}
	if (exp >= 100) {
		buf[size++] = (exp / 100) % 10 + '0';
	}
	buf[size++] = (exp / 10) % 10 + '0';
	buf[size++] = (exp / 1) % 10 + '0';
	state->body.size = size;
}

static void cvt_e(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int exp = 0;
	if (dtoa(state, spec, num, &exp)) return;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	if (set_sigfigs(state, precision + 1)) {
		++exp;
	}
	emit_e(state, spec, exp);
}

static void emit_f(
		struct format_state *state, struct spec *spec, int exp, int precision)
{
	char *buf = state->buffer;
	int size = state->body.size;
	// K represents the location of the decimal point relative to the end of
	// the string we just generated.
	if (exp < 0) {
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
		state->leading_zeros = exp + 1;
		if (state->leading_zeros < precision) {
			// If there's room for more digits, use the ones we generated.
			int available = precision - state->leading_zeros;
			if (size > available) {
				size = available;
			}
		} else {
			// Don't generate more zeros than we actually need to show.
			state->leading_zeros = precision;
			size = 0;
		}
		if (spec->flags & FLAG_TRIM_ZEROS) {
			size = trim_zeros(buf, size);
		}
	} else if (exp < size) {
		// The decimal point should be located somewhere within the string.
		// Move the characters, then either round or pad to make the number
		// of digits following the decimal equal to the precision requested.
		int leading = exp + 1;
		char *pos = buf + leading;
		memmove(pos + 1, pos, size - leading);
		*pos = '.';
		++size;
		if (spec->flags & FLAG_TRIM_ZEROS) {
			size = trim_zeros(buf, size);
		}
	} else {
		// The decimal point appears somewhere to the right of the string.
		// This implies that all the fractional digits are also zeros.
		state->trailing_zeros = exp - size + 1;
		if (0 == (spec->flags & FLAG_TRIM_ZEROS)) {
			state->trailing_point = true;
			state->trailing_fraction = precision;
		}
	}
	state->body.size = size;
}

static void cvt_f(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int exp = 0;
	if (dtoa(state, spec, num, &exp)) return;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	while (set_sigfigs(state, precision + exp + 1)) {
		++exp;
	}
	emit_f(state, spec, exp, precision);
}

static void cvt_g(struct format_state *state, struct spec *spec, va_list *arg)
{
	double num = va_arg(*arg, double);
	int exp = 0;
	if (dtoa(state, spec, num, &exp)) return;
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	if (precision < 1) {
		precision = 1;
	}
	if (set_sigfigs(state, precision)) {
		++exp;
	}
	// With mode %g, precision controls the total number of significant digits,
	// not (as with %e and %f) the number of digits after the decimal point.
	// We'll print very large or small numbers using scientific notation, while
	// numbers closer to zero will be printed as simple decimals.
	spec->flags |= FLAG_TRIM_ZEROS;
	if (exp < -4 || exp >= precision) {
		emit_e(state, spec, exp);
	} else {
		emit_f(state, spec, exp, precision);
	}
}

static void cvt_a(struct format_state *state, struct spec *spec, va_list *arg)
{
	union {
		double d;
		uint64_t u;
	} num;
	num.d = va_arg(*arg, double);
	sign_prefix(state, spec, num.d < 0.0);
	bool upper = spec->flags & FLAG_UPPERCASE;
	char *buf = state->buffer;
	*buf++ = '0';
	*buf++ = upper? 'X': 'x';
	// The first digit is 1 for normalized, 0 for denormalized. Most numbers
	// will be normalized.
	const uint64_t expmask =   0x7FF0000000000000LLU;
	bool normal = (num.u & expmask);
	*buf++ = normal? '1': '0';
	*buf++ = '.';
	// Print the fractional part of the value out as an ordinary hex literal,
	// with as many digits as the requested precision, up to the total number
	// of digits available.
	int precision = spec->flags & FLAG_HAS_PRECISION? spec->precision: 6;
	const char *alpha = upper? "0123456789ABCDEF": "0123456789abcdef";
	const uint64_t fracmask =  0x000FFFFFFFFFFFFFLLU;
	uint64_t frac = num.u & fracmask;
	for (unsigned i = 0; i < 12 && i < precision; ++i) {
		int val = (frac >> ((12 - i)*4)) & 0x0F;
		*buf++ = alpha[val];
	}
	// Trim off any trailing zeros.
	while (buf[-1] == '0') {
		--buf;
	}
	*buf++ = upper? 'P': 'p';
	// Print the exponent as a one, two, or three-digit decimal.
	int exp = ((num.u & expmask) >> 52) - 1023;
	*buf++ = (exp < 0)? '-': '+';
	exp = ulabs(exp);
	if (exp >= 100) *buf++ = alpha[(exp / 100) % 10];
	if (exp >= 10) *buf++ = alpha[(exp / 10) % 10];
	*buf++ = alpha[exp % 10];
	state->body.addr = state->buffer;
	state->body.size = buf - state->buffer;
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
		case 'a':
		case 'A': cvt_a(state, &spec, arg); break;
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

static bool outpad(struct format_state *state, const char *src, size_t *len)
{
	if (0 == *len) return false;
	state->current.addr = src;
	state->current.size = (*len <= MAX_PADDING)? *len: MAX_PADDING;
	*len -= state->current.size;
	return true;
}

static bool outchunk(struct format_state *state, const struct format_chunk *chk)
{
	if (0 == chk->size) return false;
	state->current = *chk;
	return true;
}

bool _format_next(struct format_state *state, va_list *arg)
{
	while (1) {
		if (outpad(state, padding, &state->leading_spaces)) return true;
		if (outchunk(state, &prefix_chunk[state->prefix])) {
			state->prefix = PREFIX_NONE;
			return true;
		}
		if (outpad(state, zeros, &state->leading_zeros)) return true;
		if (outchunk(state, &state->body)) {
			state->body.size = 0;
			return true;
		}
		if (outpad(state, zeros, &state->trailing_zeros)) return true;
		if (state->trailing_point) {
			state->current.addr = ".";
			state->current.size = 1;
			state->trailing_point = false;
			return true;
		}
		if (outpad(state, zeros, &state->trailing_fraction)) return true;
		if (outpad(state, padding, &state->trailing_spaces)) return true;
		switch (*state->fmt) {
			case '%': convert(state, arg); break;
			case '\0': return false;
			default: scan_literal(state); break;
		}
	}
	return false;
}

#ifdef TESTSUITE
static char *enfmt(const char *fmt, ...)
{
	static struct format_state st;
	_format_start(&st, fmt);
	va_list arg;
	va_start(arg, fmt);
	static char buf[1024];
	char *dest = buf;
	while (_format_next(&st, &arg)) {
		memcpy(dest, st.current.addr, st.current.size);
		dest += st.current.size;
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
	CHECK_STR(enfmt("%g", 0.99999999999999), "1", size);
	CHECK_STR(enfmt("%g", 1.9999999999999), "2", size);
	CHECK_STR(enfmt("%g", 999999999999999.0), "1e+15", size);
	CHECK_STR(enfmt("%g", 19999999999999.0), "2e+13", size);
	CHECK_STR(enfmt("%e", 0.99999999999999), "1.000000e+00", size);
	CHECK_STR(enfmt("%e", 1.9999999999999), "2.000000e+00", size);
	CHECK_STR(enfmt("%e", 999999999999999.0), "1.000000e+15", size);
	CHECK_STR(enfmt("%e", 19999999999999.0), "2.000000e+13", size);
	CHECK_STR(enfmt("%f", 0.99999999999999), "1.000000", size);
	CHECK_STR(enfmt("%f", 0.099999999999999), "0.100000", size);
	CHECK_STR(enfmt("%f", 9.99999999999999), "10.000000", size);
	CHECK_STR(enfmt("%f", 1.000000), "1.000000", size);
	CHECK_STR(enfmt("%f", 0.100000), "0.100000", size);
	CHECK_STR(enfmt("%f", 10.000000), "10.000000", size);
	CHECK_STR(enfmt("%f", 1.9999999999999), "2.000000", size);
	CHECK_STR(enfmt("%f", 999999999999999.0), "999999999999999.000000", size);
	CHECK_STR(enfmt("%f", 19999999999999.0), "19999999999999.000000", size);
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
	CHECK_STR(enfmt("%a", 0x3d0edbafa0000000), "0x1.edbafap-47", size);
	CHECK_STR(enfmt("%a", 0x3e795f4f60000000), "0x1.95f4f6p-24", size);
	CHECK_STR(enfmt("%a", 0x3c79f17700000000), "0x1.9f177p-56", size);
	CHECK_STR(enfmt("%a", 0x388c030dc0000000), "0x1.c030dcp-119", size);
	CHECK_STR(enfmt("%a", 0x3f4da10e60000000), "0x1.da10e6p-11", size);
	CHECK_STR(enfmt("%a", 0x3cdf61b3c0000000), "0x1.f61b3cp-50", size);
	CHECK_STR(enfmt("%a", 0x3ff2f0d740000000), "0x1.2f0d74p+0", size);
	CHECK_STR(enfmt("%a", 0x455ef14ba0000000), "0x1.ef14bap+86", size);
	CHECK_STR(enfmt("%a", 0x475009e600000000), "0x1.009e6p+118", size);
	CHECK_STR(enfmt("%a", 0x3e415d0ba0000000), "0x1.15d0bap-27", size);
	CHECK_STR(enfmt("%a", 0x3d19b162e0000000), "0x1.9b162ep-46", size);
	CHECK_STR(enfmt("%a", 0x451ced94a0000000), "0x1.ced94ap+82", size);
	CHECK_STR(enfmt("%a", 0x4106f18120000000), "0x1.6f1812p+17", size);
	CHECK_STR(enfmt("%a", 0x390f1a74c0000000), "0x1.f1a74cp-111", size);
	CHECK_STR(enfmt("%a", 0x3f6259e5c0000000), "0x1.259e5cp-9", size);
	CHECK_STR(enfmt("%a", 0x38de210d40000000), "0x1.e210d4p-114", size);
	CHECK_STR(enfmt("%a", 0x4307881720000000), "0x1.788172p+49", size);
	CHECK_STR(enfmt("%a", 0x388c62d9a0000000), "0x1.c62d9ap-119", size);
	CHECK_STR(enfmt("%a", 0x38a68efb60000000), "0x1.68efb6p-117", size);
	CHECK_STR(enfmt("%a", 0x433498e2c0000000), "0x1.498e2cp+52", size);
	CHECK_STR(enfmt("%a", 0x3fb048d9a0000000), "0x1.048d9ap-4", size);
	CHECK_STR(enfmt("%a", 0x3d9acd3040000000), "0x1.acd304p-38", size);
	CHECK_STR(enfmt("%a", 0x453aac2e60000000), "0x1.aac2e6p+84", size);
	CHECK_STR(enfmt("%a", 0x41b89575e0000000), "0x1.89575ep+28", size);
}
#endif

