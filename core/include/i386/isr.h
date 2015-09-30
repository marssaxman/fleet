#ifndef _I386_ISR_H
#define _I386_ISR_H

#include <stdint.h>

// This module defines an array of ISR stubs which will set up a common stack
// layout and invoke the master ISR, which should be defined in a C module.
// The master ISR will get a pointer to the saved register state on the stack.
struct _isr_state {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interrupt_number;
	uint32_t error_code;
	uint32_t return_address;
};
extern void _isr(struct _isr_state*);

// The ISR stubs are spaced 16 bytes apart for ease of indexing.
struct _isr_stub {
	uint8_t code[16];
};
extern struct _isr_stub _isr_stubs[256];

#endif //_I386_ISR_H
