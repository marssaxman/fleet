#include "irq.h"
#include "log.h"
#include "pic.h"

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

#define IRQ_COUNT 16
static struct work_queue _handlers[IRQ_COUNT];
static unsigned irq_enable_mask = 0;

void irq_init()
{
	for (unsigned i = 0; i < IRQ_COUNT; ++i) {
		work_queue_init(&_handlers[i]);
	}
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void irq_listen(unsigned irq, struct work_item *handler)
{
	queue_work(&_handlers[irq], handler);
	irq_enable_mask |= 1 << irq;
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void _irq(unsigned irq)
{
	_log_printf("IRQ %d\n", irq);
	await_queue(&_handlers[irq]);
	irq_enable_mask &= ~(1 << irq);
	_pic_set_irqs(irq_enable_mask);
}

