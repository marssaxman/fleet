// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <setjmp.h>
#include "stdio/stream.h"
#include "libc/main.h"
#include "uart.h"
#include "log.h"

extern int main(int argc, char *argv[]);

static struct _stream _stdin;
FILE *stdin = &_stdin;
static struct _stream _stdout;
FILE *stdout = &_stdout;
static struct _stream _stderr;
FILE *stderr = &_stderr;

static jmp_buf exitjmp;
static int ret_status;

int _main(const char *cmdline)
{
	_init_stream(&_stdin);
	_init_stream(&_stdout);
	_init_stream(&_stderr);
	_stdin.id = _uart_open(&COM1);
	_stdout.id = _uart_open(&COM2);
	_stderr.id = _log_open();
	if (0 == setjmp(exitjmp)) {
		ret_status = main(0, 0);
	}
	fflush(0);
	return ret_status;
}

void _Exit(int status)
{
	ret_status = status;
	longjmp(exitjmp, 1);
}
