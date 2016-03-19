// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <stdarg.h>
#include "scanner.h"

int vsscanf(const char *buffer, const char *format, va_list arg)
{
	struct scanner_state st;
	_scanner_init(&st, format, &arg);
	do {
		if ('\0' == *buffer) break;
	} while (_scanner_next(&st, *buffer++));
	return st.result;
}

