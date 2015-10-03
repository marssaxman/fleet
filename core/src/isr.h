#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

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

// The exception stubs handle CPU interrupts and invoke _exception.
extern void _exception(struct _isr_state*);
void _isr_exc00();
void _isr_exc01();
void _isr_exc02();
void _isr_exc03();
void _isr_exc04();
void _isr_exc05();
void _isr_exc06();
void _isr_exc07();
void _isr_exc08();
void _isr_exc09();
void _isr_exc0A();
void _isr_exc0B();
void _isr_exc0C();
void _isr_exc0D();
void _isr_exc0E();
void _isr_exc0F();
void _isr_exc10();
void _isr_exc11();
void _isr_exc12();
void _isr_exc13();

// The IRQ stubs handle device IRQs and call _irq.
extern void _irq(uint32_t irq);
void _isr_irq0();
void _isr_irq1();
void _isr_irq2();
void _isr_irq3();
void _isr_irq4();
void _isr_irq5();
void _isr_irq6();
void _isr_irq7();
void _isr_irq8();
void _isr_irq9();
void _isr_irqA();
void _isr_irqB();
void _isr_irqC();
void _isr_irqD();
void _isr_irqE();
void _isr_irqF();

#endif //_ISR_H
