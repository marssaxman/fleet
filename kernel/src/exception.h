// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>

struct cpu_state {
	uint32_t edi, esi, ebp, esp;
	uint32_t ebx, edx, ecx, eax;
	uint32_t error, eip, cs, eflags;
};

void _exception(unsigned code, struct cpu_state*);

#endif //EXCEPTION_H

