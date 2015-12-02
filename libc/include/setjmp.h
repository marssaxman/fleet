// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _SETJMP_H
#define _SETJMP_H

typedef unsigned long jmp_buf[6];
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

#endif //_SETJMP_H
