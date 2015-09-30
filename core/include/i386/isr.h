#ifndef _I386_ISR_H
#define _I386_ISR_H

#include <stdint.h>

// This module defines an array of ISR stubs which will set up a common stack
// layout and invoke the master ISR, which should be defined in a C module.
// The master ISR will get a pointer to the saved register state on the stack.
struct _isr_state {
	// Registers saved with PUSHAL. Note that esp here is the old value of
	// esp, which should be the address of the interrupt_number field.
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	// Vector number pushed by stub routine
	uint32_t interrupt_number;
	// Error data pushed by interrupt, or dummy pushed by stub routine
	uint32_t error_code;
	// Previous execution state, saved by interrupt gate
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
};
extern void _isr(struct _isr_state*);

// The ISR stubs are spaced 16 bytes apart for ease of indexing.
struct _isr_stub {
	uint8_t code[16];
};
extern struct _isr_stub _isr_stubs[256];

#endif //_I386_ISR_H
