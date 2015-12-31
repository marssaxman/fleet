// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "scanner.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

static inline bool isodigit(char c) { return c >= '0' && c <= '7'; }

// return codes for pattern mode functions
enum {
	MATCH_FAIL = 0, // error: char does not belong here
	MATCH_ACCEPT = 1, // accept char, continue with the current pattern
	MATCH_RETRY = 2, // current pattern is done, apply char to next pattern
	MATCH_NEXT = 3, // accept char, then advance to next pattern on next char
};

// bitmask flag values
enum {
	FLAG_NEGATIVE = 1,
	FLAG_UNSIGNED = 2,
	FLAG_HAS_WIDTH = 4,
};

typedef int (*mode_t)(struct scanner_state*, char c);

static int mode_space(struct scanner_state *s, char c)
{
	return isspace(c)? MATCH_ACCEPT: MATCH_RETRY;
}

static int add_digit(struct scanner_state *s, char c, int radix)
{
	unsigned dig = 0;
	if (c >= '0' && c <= '9') dig = c - '0';
	else if (c >= 'a' && c <= 'f') dig = c - 'a' + 0x0A;
	else if (c >= 'A' && c <= 'F') dig = c - 'A' + 0x0A;
	else return MATCH_FAIL;
	if (dig >= radix) return MATCH_FAIL;
	if (s->dest) {
		void *d = s->dest;
		if (s->flags & FLAG_UNSIGNED) {
			switch (s->length) {
				case 1: *(uint8_t*)d = *(uint8_t*)d * radix + dig; break;
				case 2: *(uint16_t*)d = *(uint16_t*)d * radix + dig; break;
				case 4: *(uint32_t*)d = *(uint32_t*)d * radix + dig; break;
				case 8: *(uint64_t*)d = *(uint64_t*)d * radix + dig; break;
				default: *(unsigned*)d = *(unsigned*)d * radix + dig; break;
			}
		} else {
			switch (s->length) {
				case 1: *(int8_t*)d = *(int8_t*)d * radix + dig; break;
				case 2: *(int16_t*)d = *(int16_t*)d * radix + dig; break;
				case 4: *(int32_t*)d = *(int32_t*)d * radix + dig; break;
				case 8: *(int64_t*)d = *(int64_t*)d * radix + dig; break;
				default: *(int*)d = *(int*)d * radix + dig; break;
			}
		}
	}
	return MATCH_ACCEPT;
}

static int non_digit(struct scanner_state *s)
{
	if (s->flags & FLAG_NEGATIVE && s->dest) {
		switch (s->length) {
			case 1: *(int8_t*)s->dest *= -1; break;
			case 2: *(int16_t*)s->dest *= -1; break;
			case 4: *(int32_t*)s->dest *= -1; break;
			case 8: *(int64_t*)s->dest *= -1; break;
			default: *(int*)s->dest *= -1; break;
		}
	}
	return MATCH_RETRY;
}

static int mode_dec_more(struct scanner_state *s, char c)
{
	return isdigit(c)? add_digit(s, c, 10): non_digit(s);
}

static int mode_dec_digit(struct scanner_state *s, char c)
{
	s->mode = mode_dec_more;
	if (isdigit(c)) return add_digit(s, c, 10);
	return MATCH_FAIL;
}

static int mode_hex_more(struct scanner_state *s, char c)
{
	return isxdigit(c)? add_digit(s, c, 16): non_digit(s);
}

static int mode_hex_digit(struct scanner_state *s, char c)
{
	s->mode = mode_hex_more;
	if (isxdigit(c)) return add_digit(s, c, 16);
	return MATCH_FAIL;
}

static int mode_oct_more(struct scanner_state *s, char c)
{
	return isodigit(c)? add_digit(s, c, 8): non_digit(s);
}

static int mode_oct_digit(struct scanner_state *s, char c)
{
	s->mode = mode_oct_more;
	if (isodigit(c)) return add_digit(s, c, 8);
	return MATCH_FAIL;
}

static int mode_xo_prefix(struct scanner_state *s, char c)
{
	if (c == 'x' || c == 'X') {
		s->mode = mode_hex_digit;
		return MATCH_ACCEPT;
	}
	return mode_oct_digit(s, c);
}

static int mode_i_prefix(struct scanner_state *s, char c)
{
	if (c == '0') {
		s->mode = mode_xo_prefix;
		return MATCH_ACCEPT;
	}
	return mode_dec_digit(s, c);
}

static int match_sign(struct scanner_state *s, char c, mode_t other)
{
	s->mode = other;
	switch (c) {
		case '-': s->flags |= FLAG_NEGATIVE;
		case '+': return MATCH_ACCEPT;
		default: return other(s, c);
	}
}

static int mode_i(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return match_sign(s, c, mode_i_prefix);
}

static int mode_d(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return match_sign(s, c, mode_dec_digit);
}

static int mode_u(struct scanner_state *s, char c)
{
	s->flags |= FLAG_UNSIGNED;
	return mode_d(s, c);
}

static int mode_o(struct scanner_state *s, char c)
{
	s->flags |= FLAG_UNSIGNED;
	if (isspace(c)) return MATCH_ACCEPT;
	return match_sign(s, c, mode_oct_digit);
}

static int mode_x(struct scanner_state *s, char c)
{
	s->flags |= FLAG_UNSIGNED;
	if (isspace(c)) return MATCH_ACCEPT;
	return match_sign(s, c, mode_hex_digit);
}

static int mode_f(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return MATCH_FAIL;
}

static int mode_e(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return MATCH_FAIL;
}

static int mode_g(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return MATCH_FAIL;
}

static int mode_a(struct scanner_state *s, char c)
{
	if (isspace(c)) return MATCH_ACCEPT;
	return MATCH_FAIL;
}

static int mode_c(struct scanner_state *s, char c)
{
	if (s->flags & FLAG_HAS_WIDTH) {
		if (0 >= s->width) return MATCH_RETRY;
		--s->width;
	}
	if (s->dest) *(char*)s->dest++ = c;
	return (s->flags & FLAG_HAS_WIDTH)? MATCH_ACCEPT: MATCH_NEXT;
}

static int mode_s(struct scanner_state *s, char c)
{
	bool done = false;
	if (isspace(c)) done = true;
	else if (s->flags & FLAG_HAS_WIDTH) done = !s->width--;
	if (s->dest) *(char*)s->dest++ = done? '\0': c;
	return done? MATCH_RETRY: MATCH_ACCEPT;
}

static int mode_p(struct scanner_state *s, char c)
{
	return MATCH_FAIL;
}

static int mode_scanset(struct scanner_state *s, char c)
{
	return MATCH_FAIL;
}

static int mode_negset(struct scanner_state *s, char c)
{
	return MATCH_FAIL;
}

static int mode_n(struct scanner_state *s, char c)
{
	return MATCH_FAIL;
}

static int mode_literal(struct scanner_state *s, char c)
{
	return c == *s->specifier? MATCH_NEXT: MATCH_FAIL;
}

static int mode_done(struct scanner_state *s, char c)
{
	return MATCH_FAIL;
}

static void parse_dest(struct scanner_state *state)
{
	if ('*' == *state->format) {
		// Don't write the scanned value out to an argument.
		++state->format;
	} else {
		// Get the next argument.
		state->dest = va_arg(*state->arg, void*);
	}
}

static void parse_width(struct scanner_state *state)
{
	const char *fmt = state->format;
	while (*fmt >= '0' && *fmt <= '9') {
		state->width = state->width * 10 + *fmt++ - '0';
		state->flags |= FLAG_HAS_WIDTH;
	}
	state->format = fmt;
}

static void parse_length(struct scanner_state *state)
{
	const char *fmt = state->format;
	switch (*fmt++) {
		case 'h':
			state->length = sizeof(short);
			if ('h' == *fmt) {
				fmt++;
				state->length = sizeof(char);
			}
			break;
		case 'l':
			state->length = sizeof(long);
			if ('l' == *fmt) {
				fmt++;
				state->length = sizeof(long long);
			}
			break;
		case 'j':
			state->length = sizeof(intmax_t);
			break;
		case 'z':
			state->length = sizeof(size_t);
			break;
		case 't':
			state->length = sizeof(ptrdiff_t);
			break;
		case 'L':
			state->length = sizeof(long double);
			break;
		default: --fmt;
	}
	state->format = fmt;
}

static void parse_scanset(struct scanner_state *state)
{
	if ('^' == *++state->format) {
		state->mode = mode_negset;
		++state->format;
	} else {
		state->mode = mode_scanset;
	}
	state->specifier = state->format;
	state->spec_size = 0;
	while (1) {
		switch (*state->format) {
			case ']': ++state->format;
			case '\0': return;
			default: ++state->spec_size;
		}
		++state->format;
	}
}

static void parse_specifier(struct scanner_state *state)
{
	state->specifier = state->format;
	state->spec_size = 1;
	switch (*state->format) {
		case '\0': return;
		case 'i': state->mode = mode_i; break;
		case 'u': state->mode = mode_u; break;
		case 'd': state->mode = mode_d; break;
		case 'o': state->mode = mode_o; break;
		case 'x': state->mode = mode_x; break;
		case 'f': state->mode = mode_f; break;
		case 'e': state->mode = mode_e; break;
		case 'g': state->mode = mode_g; break;
		case 'a': state->mode = mode_a; break;
		case 'c': state->mode = mode_c; break;
		case 's': state->mode = mode_s; break;
		case 'p': state->mode = mode_p; break;
		case 'n': state->mode = mode_n; break;
		case '[': parse_scanset(state); return;
		case '%':
		default: state->mode = mode_literal;
	}
	++state->format;
}

static void parse(struct scanner_state *state)
{
	state->width = 0;
	state->length = 0;
	state->spec_size = 0;
	state->dest = 0;
	state->flags = 0;
	state->mode = mode_done;
	state->specifier = state->format;
	char ch = *state->format;
	if (0 == ch) return;
	++state->format;
	if ('\0' == ch) {
		state->mode = mode_done;
	} else if (isspace(ch)) {
		state->mode = mode_space;
		state->spec_size = 1;
	} else if ('%' == ch) {
		// %[*][width][length]specifier
		parse_dest(state);
		parse_width(state);
		parse_length(state);
		parse_specifier(state);
	} else {
		state->mode = mode_literal;
		state->spec_size = 1;
	}
}

void _scanner_init(struct scanner_state *state, const char *fmt, va_list *arg)
{
	memset(state, 0, sizeof(struct scanner_state));
	state->format = fmt;
	state->arg = arg;
	parse(state);
}

bool _scanner_next(struct scanner_state *state, char ch)
{
	do switch (state->mode(state, ch)) {
		case MATCH_RETRY:
			if (state->dest) ++state->result;
			parse(state);
			continue;
		case MATCH_NEXT:
			if (state->dest) ++state->result;
			parse(state);
			return true;
		case MATCH_ACCEPT:
			return true;
		case MATCH_FAIL:
			state->mode = mode_done;
			return false;
	} while (1);
}


