// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

struct multiboot_info;
void _memory_init(struct multiboot_info *info);
void *_memory_alloc(size_t bytes);
void *sbrk(intptr_t increment);

#endif // MEMORY_H
