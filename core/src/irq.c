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

struct _task _handlers[16];

static unsigned irq_enable_mask = 0;

void irq_init()
{
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void irq_listen(unsigned irq, struct _task *handler)
{
	_task_schedule(&_handlers[irq], handler);
	irq_enable_mask |= 1 << irq;
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void _irq(unsigned irq)
{
	_log_printf("IRQ %d\n", irq);
	_task_execute(&_handlers[irq]);
	irq_enable_mask &= ~(1 << irq);
	_pic_set_irqs(irq_enable_mask);
}

