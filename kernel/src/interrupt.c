// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "interrupt.h"
#include "debug.h"
#include "pic.h"

struct irq_action *handlers[16];

void _interrupt_init() {
	_pic_mask(0);
}

void _irq_attach(unsigned irq, struct irq_action *action) {
	action->next = handlers[irq];
	handlers[irq] = action;
}

void _interrupt(int vector) {
	if (vector >= 32 && vector < 48) {
		int irq = vector - 32;
		struct irq_action *item = handlers[irq];
		while (item) {
			struct irq_action *next = item->next;
			item->isr(item);
			item = next;
		}
		_pic_eoi();
	} else {
		_panic("unexpected interrupt #%d\n", vector);
	}
}

