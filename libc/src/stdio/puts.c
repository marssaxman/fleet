// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>

int puts(const char *str)
{
	int out = fputs(str, stdout);
	if (out < 0) return out;
	if (0 == putchar('\n')) return EOF;
	return out + 1;
}

