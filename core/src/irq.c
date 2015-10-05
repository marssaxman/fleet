#include "irq.h"
#include "log.h"
#include "pic.h"
#include "isr.h"
#include <startc/idt.h>

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
static struct signal _irqs[IRQ_COUNT];
static unsigned irq_enable_mask = 0;

void irq_listen(unsigned irq, struct signal_action *handler)
{
	signal_listen(&_irqs[irq], handler);
	irq_enable_mask |= 1 << irq;
	_log_printf("setting IRQ enable mask %d\n", irq_enable_mask);
	_pic_set_irqs(irq_enable_mask);
}

void _irq(unsigned irq)
{
	_log_printf("IRQ %d\n", irq);
	signal_raise(&_irqs[irq]);
	irq_enable_mask &= ~(1 << irq);
	_pic_set_irqs(irq_enable_mask);
}

static void register_isr(struct _idt_entry *gate, void *ptr)
{
	uint32_t addr = (uint32_t)ptr;
	gate->offset0_15 = addr & 0x0000FFFF;
	gate->offset16_31 = (addr >> 16) & 0x0000FFFF;
	gate->selector = 0x08; // the only code segment we use
	gate->flags = 0x8E;	// 32-bit interrupt gate
}

void _irq_init()
{
	// Register an ISR for each device IRQ.
	register_isr(&_idt[0x20], _isr_irq0);
	register_isr(&_idt[0x21], _isr_irq1);
	register_isr(&_idt[0x22], _isr_irq2);
	register_isr(&_idt[0x23], _isr_irq3);
	register_isr(&_idt[0x24], _isr_irq4);
	register_isr(&_idt[0x25], _isr_irq5);
	register_isr(&_idt[0x26], _isr_irq6);
	register_isr(&_idt[0x27], _isr_irq7);
	register_isr(&_idt[0x28], _isr_irq8);
	register_isr(&_idt[0x29], _isr_irq9);
	register_isr(&_idt[0x2A], _isr_irqA);
	register_isr(&_idt[0x2B], _isr_irqB);
	register_isr(&_idt[0x2C], _isr_irqC);
	register_isr(&_idt[0x2D], _isr_irqD);
	register_isr(&_idt[0x2E], _isr_irqE);
	register_isr(&_idt[0x2F], _isr_irqF);

	// Prepare a work queue for each IRQ.
	for (unsigned i = 0; i < IRQ_COUNT; ++i) {
		signal_init(&_irqs[i]);
	}

	// Set the initial IRQ enable state.
	_pic_set_irqs(irq_enable_mask);
}

