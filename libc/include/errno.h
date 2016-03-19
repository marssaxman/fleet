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

// System call not implemented
#define ENOSYS 4

// Additional implementation-specific error codes allowed by the standard
#define EINVAL 5
#define ENOENT 6
#define ESPIPE 7
#define EROFS 8
#define EBADF 9
#define EPERM 10
#define EMFILE 11
#define EISCONN 12

#endif //_ERRNO_H

