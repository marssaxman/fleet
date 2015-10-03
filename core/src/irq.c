#include "irq.h"
#include "log.h"

#define IRQS 16
const unsigned irq_count = IRQS;

/*
	0 = timer
	1 = keyboard
	2 = cascade (never raised)
	3 = com2
	4 = com1
	5 = lpt2
	6 = floppy disk
	7 = lpt1, unreliable spurious interrupt
	8 = real-time clock
	9 = peripherals
	A = peripherals
	B = peripherals
	C = PS2 mouse
	D = FPU / coprocessor
	E = primary hard disk
	F = secondary hard disk
*/

void irq_attach(unsigned irq, struct irq_handler *entry)
{
}

void irq_detach(struct irq_handler *entry)
{
}

void _irq(unsigned irq)
{
	_log_printf("IRQ %d\n", irq);
}

