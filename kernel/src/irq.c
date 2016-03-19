// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "irq.h"
#include "log.h"
#include "interrupt.h"

/*
	0 = timer
	1 = keyboard
	2 = cascade (never raised, should be permanently enabled)
	3 = com2
	4 = com1
	5 = lpt2
	6 = floppy disk
	7 = lpt1, unreliable spurious interrupt
	8 = real-time clock
	9 = peripherals
	A = peripherals
	B = peripherals
	C = PS2 mouse
	D = FPU / coprocessor
	E = primary hard disk
	F = secondary hard disk
*/

#define IRQ_COUNT 16
static struct {
	void (*action)(void*);
	void *ref;
} handlers[IRQ_COUNT];

// Always enable IRQ2 so we can receive the PIC2 cascade IRQs.
static unsigned irq_enable_mask = 0x0004;

void _irq_listen(unsigned irq, void *ref, void (*action)(void*))
{
	handlers[irq].action = 0;
	handlers[irq].ref = ref;
	handlers[irq].action = action;
	irq_enable_mask |= 1 << irq;
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void _irq_ignore(unsigned irq)
{
	handlers[irq].action = 0;
}

void _isr_irq(unsigned irq, struct _cpu_state *state)
{
	_log_printf("IRQ %d\n", irq);
	// suppress further interrupts on this IRQ unless some action requests one
	irq_enable_mask &= ~(1 << irq);
	_pic_set_irqs(irq_enable_mask);
	if (handlers[irq].action) {
		handlers[irq].action(handlers[irq].ref);
	}
}

void _irq_init()
{
	// Set the initial IRQ enable state.
	_pic_set_irqs(irq_enable_mask);
}

