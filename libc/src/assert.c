// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <stdlib.h>

void __assert(const char *expr, const char *file, int line, const char *func)
{
	static const char *msg = "Assertion \"%s\" failed at %s:%d)\n";
	fprintf(stderr, msg, expr, file, func, line);
	fflush(NULL);
	abort();
}
