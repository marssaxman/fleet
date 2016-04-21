// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

void _interrupt_init();
void _interrupt_enable(); // turn interrupts on
void _interrupt_disable(); // turn interrupts off

struct irq_action {
	void (*isr)(struct irq_action*);
	struct irq_action *next;
};
void _irq_attach(unsigned irq, struct irq_action*);

struct cpu_state {
	uint32_t edi, esi, ebp, esp;
	uint32_t ebx, edx, ecx, eax;
	uint32_t error, eip, cs, eflags;
};
extern void _interrupt_exception(unsigned code, struct cpu_state*);

#endif //INTERRUPT_H

