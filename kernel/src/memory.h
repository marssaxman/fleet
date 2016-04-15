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
void _memory_init(uint32_t magic, struct multiboot_info *info);

extern void *_memory_base;
extern void *_memory_break;
extern void *_memory_end;

#endif // MEMORY_H

