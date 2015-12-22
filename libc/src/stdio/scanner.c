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

enum mode {
	MODE_PARSE = 0,
	MODE_SPACE,
	MODE_INTEGER,
	MODE_DEC,
	MODE_OCT_OR_HEX,
	MODE_OCT_BEGIN,
	MODE_OCT_BODY,
	MODE_HEX_BEGIN,
	MODE_HEX_BODY,
	MODE_FLOAT,
	MODE_FIX,
	MODE_EXP,
	MODE_HEXFLOAT,
	MODE_CHAR,
	MODE_STR,
	MODE_PTR,
	MODE_SCANSET,
	MODE_NEGSET,
	MODE_NUM,
	MODE_LITERAL,
	MODE_DONE,
	NUM_MODES
};

void _scanner_init(struct scanner_state *state, const char *fmt, va_list *arg)
{
	memset(state, 0, sizeof(struct scanner_state));
	state->mode = MODE_PARSE;
	state->result = -1;
	state->format = fmt;
	state->arg = arg;
}

static void parse_dest(struct scanner_state *state)
{
	if ('*' == *state->format) {
		// Don't write the scanned value out to an argument.
		++state->format;
	} else {
		// Get the next argument.
		state->dest = va_arg(state->arg, void*);
	}
}

static void parse_width(struct scanner_state *state)
{
	const char *fmt = state->format;
	while (*fmt >= '0' && *fmt <= '9') {
		state->width = state->width * 10 + *fmt++ - '0';
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
		state->mode = MODE_NEGSET;
		++state->format;
	} else {
		state->mode = MODE_SCANSET;
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
		case 'i':
		case 'u': state->mode = MODE_INTEGER; break;
		case 'd': state->mode = MODE_DEC; break;
		case 'o': state->mode = MODE_OCT_BEGIN; break;
		case 'x': state->mode = MODE_HEX_BEGIN; break;
		case 'f': state->mode = MODE_FIX; break;
		case 'e': state->mode = MODE_EXP; break;
		case 'g': state->mode = MODE_FLOAT; break;
		case 'a': state->mode = MODE_HEXFLOAT; break;
		case 'c': state->mode = MODE_CHAR; break;
		case 's': state->mode = MODE_STR; break;
		case 'p': state->mode = MODE_PTR; break;
		case 'n': state->mode = MODE_NUM; break;
		case '[': parse_scanset(state); return;
		case '%':
		default: state->mode = MODE_LITERAL;
	}
	++state->format;
}

static void mode_parse(struct scanner_state *state)
{
	state->width = 0;
	state->length = 0;
	state->spec_size = 0;
	state->dest = 0;
	state->specifier = state->format;
	char ch = *state->format;
	if (0 == ch) return;
	++state->format;
	if (isspace(ch)) {
		state->mode = MODE_SPACE;
	} else if ('%' == ch) {
		// %[*][width][length]specifier
		parse_dest(state);
		parse_width(state);
		parse_length(state);
		parse_specifier(state);
	} else {
		state->mode = MODE_LITERAL;
	}
}

static void mode_space(struct scanner_state *state)
{
	if (isspace(state->current)) {
		state->mode = MODE_SPACE;
		state->current = 0;
	} else {
		state->mode = MODE_PARSE;
	}
}

static void mode_integer(struct scanner_state *state)
{
	// Potentially any kind of integer: we figure out which kind by looking
	// at the first digit. If it's a leading zero, it must be hex or octal.
	if ('0' == state->current) {
		state->mode = MODE_OCT_OR_HEX;
		state->current = 0;
	} else if (isdigit(state->current)) {
		state->mode = MODE_DEC;
	}
}

static void mode_dec(struct scanner_state *state)
{
}

static void mode_oct_or_hex(struct scanner_state *state)
{
	if ('x' == state->current || 'X' == state->current) {
		state->mode = MODE_HEX_BODY;
		++state->format;
	} else {
		state->mode = MODE_OCT_BODY;
	}
}

static void mode_oct_begin(struct scanner_state *state)
{
}

static void mode_oct_body(struct scanner_state *state)
{
}

static void mode_hex_begin(struct scanner_state *state)
{
}

static void mode_hex_body(struct scanner_state *state)
{
}

static void mode_float(struct scanner_state *state)
{
}

static void mode_fix(struct scanner_state *state)
{
}

static void mode_exp(struct scanner_state *state)
{
}

static void mode_hexfloat(struct scanner_state *state)
{
}

static void mode_char(struct scanner_state *state)
{
}

static void mode_str(struct scanner_state *state)
{
}

static void mode_ptr(struct scanner_state *state)
{
}

static void mode_scanset(struct scanner_state *state)
{
}

static void mode_negset(struct scanner_state *state)
{
}

static void mode_num(struct scanner_state *state)
{
}

static void mode_literal(struct scanner_state *state)
{
}

static void mode_done(struct scanner_state *state)
{
	state->current = 0;
	state->mode = MODE_DONE;
}

bool _scanner_next(struct scanner_state *state, char ch)
{
	typedef void(*proc)(struct scanner_state*);
	static proc funcs[NUM_MODES] = {
		mode_parse,
		mode_integer,
		mode_dec,
		mode_oct_or_hex,
		mode_oct_begin,
		mode_oct_body,
		mode_hex_begin,
		mode_hex_body,
		mode_float,
		mode_fix,
		mode_exp,
		mode_hexfloat,
		mode_char,
		mode_str,
		mode_ptr,
		mode_scanset,
		mode_negset,
		mode_num,
		mode_literal,
		mode_done
	};
	state->current = ch;
	while (state->current) {
		proc p = funcs[state->mode];
		state->mode = MODE_DONE;
		p(state);
	}
	return state->mode != MODE_DONE;
}


