// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

void _interrupt_init();

struct irq_action {
	void (*isr)(struct irq_action*);
	struct irq_action *next;
};
void _irq_attach(unsigned irq, struct irq_action*);

#endif //INTERRUPT_H

