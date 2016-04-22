// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef KSTRING_H
#define KSTRING_H

#include <stddef.h>

// Private implementations of certain functions from <string.h>, using other
// names so we don't conflict with the libc implementations this kernel will
// most likely be linked against.

void *_kmemmove(void *dest, const void *src, size_t len);
void *_kmemcpy(void *dest, const void *src, size_t len);
void *_kmemset(void *ptr, int value, size_t num);
int _kmemcmp(const void *ptr1, const void *ptr2, size_t num);
void *_kmemzero(void *ptr, size_t num);

#endif //KSTRING_H


