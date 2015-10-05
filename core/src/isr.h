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
extern void _isr_cpu(struct _isr_state*);
void _isr_cpu00();
void _isr_cpu01();
void _isr_cpu02();
void _isr_cpu03();
void _isr_cpu04();
void _isr_cpu05();
void _isr_cpu06();
void _isr_cpu07();
void _isr_cpu08();
void _isr_cpu09();
void _isr_cpu0A();
void _isr_cpu0B();
void _isr_cpu0C();
void _isr_cpu0D();
void _isr_cpu0E();
void _isr_cpu0F();
void _isr_cpu10();
void _isr_cpu11();
void _isr_cpu12();
void _isr_cpu13();

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
