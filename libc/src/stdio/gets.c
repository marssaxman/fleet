// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <limits.h>
#include <string.h>

char *gets(char *s)
{
	char *ret = fgets(s, INT_MAX, stdin);
	size_t len = ret? strlen(s): 0;
	if (len && s[len-1] == '\n') {
		s[len-1] = '\0';
	}
	return ret;
}
