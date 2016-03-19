// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _ASSERT_H
#define _ASSERT_H

#ifndef NDEBUG
#define assert(e) ((e) || __assert(#e, __FILE__, __LINE__, __func__))
#else
#define assert(e) ((void)0)
#endif

void __assert(const char*, const char*, int, const char*) __noreturn;

#endif //_ASSERT_H

