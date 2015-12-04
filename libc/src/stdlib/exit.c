// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>

// C spec says we must allow at least 32 atexit functions.
#define FUNC_MAX 32
static int func_count = 0;
typedef void (*exitfunc)(void);
static exitfunc func_list[FUNC_MAX];

int atexit(void (*func)(void))
{
	if (func_count >= FUNC_MAX) {
		return 1;
	}
	func_list[func_count++] = func;
}

void exit(int status)
{
	while (func_count > 0) {
		func_list[--func_count]();
	}
	_Exit(status);
}

