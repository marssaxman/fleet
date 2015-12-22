// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <stdarg.h>

int sscanf(const char *buffer, const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	int ret = vsscanf(buffer, format, arg);
	va_end(arg);
	return ret;
}

