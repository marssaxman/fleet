// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
//	puts("Hello, world!");
	fputs("Hello, world!\n", stderr);
	const char *msg = "The quick brown fox jumped over the lazy dog's back.";
	char buf[256] = {0};
	int len = strlen(msg);
	for (int i = 0; i < len; ++i) {
		buf[i] = msg[i];
		fputs(buf, stderr);
		fputc('\n', stderr);
	}
	for (int i = 0; i < len; ++i) {
		buf[i] = ' ';
		fputs(buf, stderr);
		fputc('\n', stderr);
	}
	return 0;
}

