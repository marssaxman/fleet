// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "scanner.h"

void _scanner_init(struct scanner_state *state, const char *format)
{
	state->result = EOF;
}

bool _scanner_open(struct scanner_state *state, va_list *arg)
{
	return false;
}

void _scanner_next(struct scanner_state *state, char ch)
{
}


