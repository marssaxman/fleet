#include "interrupt.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"

void interrupt_init()
{
	gdt_init();
	idt_init();
}

void interrupt_handler(struct isr_parameter_registers regs)
{
	// yahooo
}

