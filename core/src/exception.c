#include "isr.h"
#include "cpu.h"
#include <startc/idt.h>
#include "panic.h"
#include "exception.h"

void _isr_cpu(struct _isr_state *regs)
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
	if (gate->selector != 0x08) {
		_panic("FAIL! gate selector is wrong: 0x%hd\n", gate->selector);
	}
	if (gate->flags != 0x8E) {
		_panic("FAIL! gate flags are wrong: 0x%hd\n", gate->flags);
	}
//	gate->selector = 0x08; // the only code segment we use
//	gate->flags = 0x8E;	// 32-bit interrupt gate
}

void _exception_init()
{
	// Install ISRs for the CPU exceptions, since we might want to recover
	// from those or use them as a chance to manage virtual memory.
	register_isr(&_idt[0x00], _isr_cpu00);
	register_isr(&_idt[0x01], _isr_cpu01);
	register_isr(&_idt[0x02], _isr_cpu02);
	register_isr(&_idt[0x03], _isr_cpu03);
	register_isr(&_idt[0x04], _isr_cpu04);
	register_isr(&_idt[0x05], _isr_cpu05);
	register_isr(&_idt[0x06], _isr_cpu06);
	register_isr(&_idt[0x07], _isr_cpu07);
	register_isr(&_idt[0x08], _isr_cpu08);
	register_isr(&_idt[0x09], _isr_cpu09);
	register_isr(&_idt[0x0A], _isr_cpu0A);
	register_isr(&_idt[0x0B], _isr_cpu0B);
	register_isr(&_idt[0x0C], _isr_cpu0C);
	register_isr(&_idt[0x0D], _isr_cpu0D);
	register_isr(&_idt[0x0E], _isr_cpu0E);
	register_isr(&_idt[0x0F], _isr_cpu0F);
	register_isr(&_idt[0x10], _isr_cpu10);
	register_isr(&_idt[0x11], _isr_cpu11);
	register_isr(&_idt[0x12], _isr_cpu12);
	register_isr(&_idt[0x13], _isr_cpu13);
}

