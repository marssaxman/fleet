#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

// Each of the _isr_exc stubs will set up a common stack layout and invoke
// the _exception() function, passing in a state record containing the saved
// register values and interrupt condition information. The _exception()
// routine must be provided by another module.
extern void _isr_exc00();
extern void _isr_exc01();
extern void _isr_exc02();
extern void _isr_exc03();
extern void _isr_exc04();
extern void _isr_exc05();
extern void _isr_exc06();
extern void _isr_exc07();
extern void _isr_exc08();
extern void _isr_exc09();
extern void _isr_exc0A();
extern void _isr_exc0B();
extern void _isr_exc0C();
extern void _isr_exc0D();
extern void _isr_exc0E();
extern void _isr_exc0F();
extern void _isr_exc10();
extern void _isr_exc11();
extern void _isr_exc12();
extern void _isr_exc13();

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
extern void _exception(struct _isr_state*);

// Each of the _isr_irq stubs will set its corresponding bit in _irq_waiting.
// We can check these bits later to see which devices need to be polled. 
extern volatile unsigned _irq_waiting;
extern void _isr_irq0();
extern void _isr_irq1();
extern void _isr_irq2();
extern void _isr_irq3();
extern void _isr_irq4();
extern void _isr_irq5();
extern void _isr_irq6();
extern void _isr_irq7();
extern void _isr_irq8();
extern void _isr_irq9();
extern void _isr_irqA();
extern void _isr_irqB();
extern void _isr_irqC();
extern void _isr_irqD();
extern void _isr_irqE();
extern void _isr_irqF();

#endif //_ISR_H
