// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <signal.h>
#include <errno.h>
#include <stdlib.h>

typedef void (*handler_t)(int);
#define SIG_COUNT 6
static handler_t handlers[SIG_COUNT];

#define SIG_ERR ((void (*)(int))-1)
#define SIG_DFL ((void (*)(int)) 0)
#define SIG_IGN ((void (*)(int)) 1)

typedef int sig_atomic_t;

void (*signal(int sig, void (*func)(int)))(int)
{
	handler_t out = SIG_ERR;
	if (sig >= 0 && sig < SIG_COUNT) {
		out = handlers[sig];
		handlers[sig] = func;
	} else {
		errno = EINVAL;
	}
	return out;
}

int raise(int sig)
{
	handler_t func = SIG_ERR;
	if (sig >= 0 && sig < SIG_COUNT) {
		func = handlers[sig];
	}
	if (func == SIG_ERR) return EINVAL;
	else if (func == SIG_DFL) _Exit(EXIT_FAILURE);
	else if (func != SIG_IGN) func(sig);
	return 0;
}


