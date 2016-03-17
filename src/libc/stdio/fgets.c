// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>

char *fgets(char *str, int num, FILE *stream)
{
	char *buf = str;
	// sanity check: make sure there is room in the buffer for at least one
	// character.
	if (num <= 1) {
		return NULL;
	}
	int ch = '\0';
	while (--num > 0) {
		ch = fgetc(stream);
		if (ch == EOF) {
			return NULL;
		}
		*str++ = ch;
		if (ch == '\n') {
			break;
		}
	}
	*str = '\0';
	return buf;
}


