// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _ERRNO_H
#define _ERRNO_H

extern int errno;

// Error codes specified by the C99 standard
#define EDOM 1
#define ERANGE 2
#define EILSEQ 3

// Additional implementation-specific error codes allowed by the standard
#define EINVAL 4
#define ENOENT 5
#define ESPIPE 6
#define EROFS 7
#define EBADF 8
#define EPERM 9
#define EMFILE 10
#define EISCONN 11

#endif //_ERRNO_H

