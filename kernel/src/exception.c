// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "exception.h"
#include "debug.h"

void _exception(unsigned code, struct cpu_state *state) {
	_kprintf("Exception #%d", code);
	if (state->error) {
		_kprintf("(error %x)", state->error);
	}
	_kprintf(":\n");
	_kprintf("EAX:%x    EBX:%x    ECX:%x    EDX:%x\n",
			state->eax, state->ebx, state->ecx, state->edx);
	_kprintf("ESI:%x    EDI:%x    ESP:%x    EBP:%x\n",
			state->esi, state->edi, state->esp, state->ebp);
	_kprintf("             EFLAGS:%x     CS:%x    EIP:%x\n",
			state->eflags, state->cs, state->eip);
	__asm__("hlt");
}


