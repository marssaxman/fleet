// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

void _interrupt_init();
void _interrupt_enable(); // turn interrupts on
void _interrupt_disable(); // turn interrupts off
bool _interrupt_suspend(); // disable and return previous state
void _interrupt_resume(bool); // enable if previously enabled

// Override these weak symbol definitions to provide a generic exception or
// IRQ handler function

struct cpu_state {
	uint32_t edi, esi, ebp, esp;
	uint32_t ebx, edx, ecx, eax;
	uint32_t error, eip, cs, eflags;
};
extern void _interrupt_exception(unsigned code, struct cpu_state*);
extern void _interrupt_irq(unsigned irq);

#endif //INTERRUPT_H

