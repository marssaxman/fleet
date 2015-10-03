#include "isr.h"
#include "cpu.h"
#include "idt.h"
#include "log.h"
#include "pic.h"
#include "panic.h"
#include "interrupt.h"

void _exception(struct _isr_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n",
			regs->interrupt_number, regs->eip);
}

static void register_isr(struct _idt_entry *gate, void *ptr)
{
	uint32_t addr = (uint32_t)ptr;
	gate->offset_low = addr & 0x0000FFFF;
	gate->offset_high = (addr >> 16) & 0x0000FFFF;
	gate->selector = 0x08; // the only code segment we use
	gate->flags = 0x8E;	// 32-bit interrupt gate
}

void _interrupt_init()
{
	// Set up a 256-entry IDT where we can install interrupt handlers.
	// Since they're all set to zero by default, any unhandled interrupt will
	// prompt the CPU to raise a segment exception.
	static struct _idt_entry idt[256];
	static struct _idt_descriptor idtptr;
	idtptr.size = sizeof(struct _idt_entry) * 256 - 1;
	idtptr.offset = (uint32_t)idt;
	_idt_load(&idtptr);

	// Install ISRs for the CPU exceptions, since we might want to recover
	// from those or use them as a chance to manage virtual memory.
	register_isr(&idt[0x00], _isr_exc00);
	register_isr(&idt[0x01], _isr_exc01);
	register_isr(&idt[0x02], _isr_exc02);
	register_isr(&idt[0x03], _isr_exc03);
	register_isr(&idt[0x04], _isr_exc04);
	register_isr(&idt[0x05], _isr_exc05);
	register_isr(&idt[0x06], _isr_exc06);
	register_isr(&idt[0x07], _isr_exc07);
	register_isr(&idt[0x08], _isr_exc08);
	register_isr(&idt[0x09], _isr_exc09);
	register_isr(&idt[0x0A], _isr_exc0A);
	register_isr(&idt[0x0B], _isr_exc0B);
	register_isr(&idt[0x0C], _isr_exc0C);
	register_isr(&idt[0x0D], _isr_exc0D);
	register_isr(&idt[0x0E], _isr_exc0E);
	register_isr(&idt[0x0F], _isr_exc0F);
	register_isr(&idt[0x10], _isr_exc10);
	register_isr(&idt[0x11], _isr_exc11);
	register_isr(&idt[0x12], _isr_exc12);
	register_isr(&idt[0x13], _isr_exc13);

	// Configure the PIC to remap device IRQs out of the range of processor
	// exceptions, so we can give them their own set of handlers.
	_pic_init();

	// Register an ISR for each device IRQ.
	register_isr(&idt[0x20], _isr_irq0);
	register_isr(&idt[0x21], _isr_irq1);
	register_isr(&idt[0x22], _isr_irq2);
	register_isr(&idt[0x23], _isr_irq3);
	register_isr(&idt[0x24], _isr_irq4);
	register_isr(&idt[0x25], _isr_irq5);
	register_isr(&idt[0x26], _isr_irq6);
	register_isr(&idt[0x27], _isr_irq7);
	register_isr(&idt[0x28], _isr_irq8);
	register_isr(&idt[0x29], _isr_irq9);
	register_isr(&idt[0x2A], _isr_irqA);
	register_isr(&idt[0x2B], _isr_irqB);
	register_isr(&idt[0x2C], _isr_irqC);
	register_isr(&idt[0x2D], _isr_irqD);
	register_isr(&idt[0x2E], _isr_irqE);
	register_isr(&idt[0x2F], _isr_irqF);

	// Our interrupt system is ready, so let it rip.
	_pic_set_irqs(0xFFFF);
	_sti();
}

