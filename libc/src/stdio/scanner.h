// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _LIBC_INTERNAL_SCANNER_H
#define _LIBC_INTERNAL_SCANNER_H

#include <stdarg.h>
#include <stdbool.h>

struct scanner_state {
	int result;
	// internal state
	char current;
	const char *format;
	va_list *arg;
	int mode;
	int width;
	int length;
	const char *specifier;
	int spec_size;
	void *dest;
};

void _scanner_init(struct scanner_state*, const char *format, va_list *arg);
bool _scanner_next(struct scanner_state*, char ch);

#endif //_LIBC_INTERNAL_SCANNER_H

