// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

void _kvprintf(const char *format, va_list args);
void _kprintf(const char *format,...);
void _panic(const char *msg, ...);
void _kassertfail(const char *file, int line, const char *cond);
#define kassert(c) ((c)? ((void)0): _kassertfail(__FILE__, __LINE__, #c))

#endif // DEBUG_H

