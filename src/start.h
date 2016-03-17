// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef START_H
#define START_H

// After configuring the CPU state, it's time to launch the kernel.
extern void _kernel(uint32_t magic, struct multiboot_info*);

// The application may define these interrupt handler functions if it wants to
// handle CPU exceptions or external device interrupts.
struct _cpu_state;
extern void _isr_cpu(unsigned code, struct _cpu_state*);
extern void _isr_irq(unsigned irq, struct _cpu_state*);

#endif //_STARTC_ENTRY_H

