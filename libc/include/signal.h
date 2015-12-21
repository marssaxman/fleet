// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _SIGNAL_H
#define _SIGNAL_H

#define SIG_ERR ((void (*)(int))-1)
#define SIG_DFL ((void (*)(int)) 0)
#define SIG_IGN ((void (*)(int)) 1)

typedef int sig_atomic_t;

void (*signal(int sig, void (*func)(int)))(int);
int raise(int sig);

#define SIGABRT 0
#define SIGFPE 1
#define SIGILL 2
#define SIGINT 3
#define SIGSEGV 4
#define SIGTERM 5

#endif //_SIGNAL_H
