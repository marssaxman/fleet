// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef IRQ_H
#define IRQ_H

void _irq_init();
void _irq_listen(unsigned irq, void *ref, void(*action)(void*));
void _irq_ignore(unsigned irq);

#endif // IRQ_H
