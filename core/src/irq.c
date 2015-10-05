#include "irq.h"
#include "log.h"
#include "pic.h"

/*
	0 = timer
	1 = keyboard
	2 = cascade (never raised, should be permanently enabled)
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

#define IRQ_COUNT 16
static struct signal _irqs[IRQ_COUNT];
// Enable IRQ2 so we get PIC2 cascade IRQs.
static unsigned irq_enable_mask = 0x0004;

void irq_listen(unsigned irq, struct signal_action *handler)
{
	signal_listen(&_irqs[irq], handler);
	irq_enable_mask |= 1 << irq;
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void _isr_irq(unsigned irq)
{
	_log_printf("IRQ %d\n", irq);
	signal_raise(&_irqs[irq]);
	irq_enable_mask &= ~(1 << irq);
	_pic_set_irqs(irq_enable_mask);
}

void _irq_init()
{
	// Prepare a work queue for each IRQ.
	for (unsigned i = 0; i < IRQ_COUNT; ++i) {
		signal_init(&_irqs[i]);
	}
	// Set the initial IRQ enable state.
	_pic_set_irqs(irq_enable_mask);
}

