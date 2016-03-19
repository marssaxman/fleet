// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _LIBC_SYSCALLS_H
#define _LIBC_SYSCALLS_H

#include <stdint.h>
#include <c/size_t.h>

// System call interface declared weak so these symbols can be overridden when
// libc is linked against some platform-specific runtime support library.

void *sbrk(intptr_t increment) __attribute__((weak));
int write(int fd, const void *src, size_t bytes) __attribute__((weak));
int read(int fd, void *dest, size_t bytes) __attribute__((weak));
int close(int fd) __attribute__((weak));

#endif //_LIBC_SYSCALLS_H

