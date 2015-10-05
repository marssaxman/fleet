#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

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
