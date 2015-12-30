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
	MODE_DEC_PREFIX,
	MODE_DEC_DIGIT,
	MODE_OCT_OR_HEX_PREFIX,
	MODE_OCT_PREFIX,
	MODE_OCT_DIGIT,
	MODE_HEX_PREFIX,
	MODE_HEX_DIGIT,
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
		case 'd': state->mode = MODE_DEC_PREFIX; break;
		case 'o': state->mode = MODE_OCT_PREFIX; break;
		case 'x': state->mode = MODE_HEX_PREFIX; break;
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

static void parse(struct scanner_state *state)
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

bool _scanner_next(struct scanner_state *state, char ch)
{
	// Handle the char according to the current mode.
	// If the char matches the mode, and is consumed, return true.
	// If the char completed the current match, switch to MODE_PARSE before
	// returning, so the next character will advance to the next specifier.
	// If the char causes a transition to some specific mode, assign that mode,
	// then continue, so the new mode will consume the character instead.
	// If the char is not legal, break: this will change to MODE_END and
	// terminate the scan.
	do switch(state->mode) {
		case MODE_PARSE:
			parse(state);
			continue;

		case MODE_SPACE:
			if (isspace(ch)) return true;
			state->mode = MODE_PARSE;
			continue;

		case MODE_INTEGER:
			if (0 == ch) {
				state->mode = MODE_OCT_OR_HEX_PREFIX;
				continue;
			} else if (isdigit(ch)) {
				state->mode = MODE_DEC_DIGIT;
				continue;
			}
			break;

		case MODE_DEC_PREFIX:
			break;

		case MODE_DEC_DIGIT:
			break;

		case MODE_OCT_OR_HEX_PREFIX:
			if ('x' == ch || 'X' == ch) {
				state->mode = MODE_HEX_DIGIT;
				return true;
			}
			state->mode = MODE_OCT_DIGIT;
			continue;

		case MODE_OCT_PREFIX:
			if ('0' == ch) {
				state->mode = MODE_OCT_DIGIT;
				return true;
			}
			break;

		case MODE_OCT_DIGIT:
			break;

		case MODE_HEX_PREFIX:
			break;

		case MODE_HEX_DIGIT:
			break;

		case MODE_FLOAT:
			break;

		case MODE_FIX:
			break;

		case MODE_EXP:
			break;

		case MODE_HEXFLOAT:
			break;

		case MODE_CHAR:
			break;

		case MODE_STR:
			break;

		case MODE_PTR:
			break;

		case MODE_SCANSET:
			break;

		case MODE_NEGSET:
			break;

		case MODE_NUM:
			break;

		case MODE_LITERAL:
			break;

		case MODE_DONE:
			return false;

	} while (state->mode = MODE_DONE);
}


