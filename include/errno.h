// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#define EDOM 1
#define ERANGE 2
#define EILSEQ 3

#ifdef __cplusplus
}
#endif

#endif //_ERRNO_H
