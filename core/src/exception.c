#include "isr.h"
#include "cpu.h"
#include <startc/idt.h>
#include "panic.h"
#include "exception.h"

void _exception(struct _isr_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n",
			regs->interrupt_number, regs->eip);
}

static void register_isr(struct _idt_entry *gate, void *ptr)
{
	uint32_t addr = (uint32_t)ptr;
	gate->offset0_15 = addr & 0x0000FFFF;
	gate->offset16_31 = (addr >> 16) & 0x0000FFFF;
	gate->selector = 0x08; // the only code segment we use
	gate->flags = 0x8E;	// 32-bit interrupt gate
}

void _exception_init()
{
	// Install ISRs for the CPU exceptions, since we might want to recover
	// from those or use them as a chance to manage virtual memory.
	register_isr(&_idt[0x00], _isr_exc00);
	register_isr(&_idt[0x01], _isr_exc01);
	register_isr(&_idt[0x02], _isr_exc02);
	register_isr(&_idt[0x03], _isr_exc03);
	register_isr(&_idt[0x04], _isr_exc04);
	register_isr(&_idt[0x05], _isr_exc05);
	register_isr(&_idt[0x06], _isr_exc06);
	register_isr(&_idt[0x07], _isr_exc07);
	register_isr(&_idt[0x08], _isr_exc08);
	register_isr(&_idt[0x09], _isr_exc09);
	register_isr(&_idt[0x0A], _isr_exc0A);
	register_isr(&_idt[0x0B], _isr_exc0B);
	register_isr(&_idt[0x0C], _isr_exc0C);
	register_isr(&_idt[0x0D], _isr_exc0D);
	register_isr(&_idt[0x0E], _isr_exc0E);
	register_isr(&_idt[0x0F], _isr_exc0F);
	register_isr(&_idt[0x10], _isr_exc10);
	register_isr(&_idt[0x11], _isr_exc11);
	register_isr(&_idt[0x12], _isr_exc12);
	register_isr(&_idt[0x13], _isr_exc13);
}

