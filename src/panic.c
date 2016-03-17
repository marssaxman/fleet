// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "panic.h"
#include "log.h"

void _panic(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	_log_print("panic: ");
	_log_vprintf(msg, args);
	while (1) {}
}

void _kassertfail(const char *file, int line, const char *cond)
{
	_panic("assertion failed at %s:%d: %s\n", file, line, cond);
}

