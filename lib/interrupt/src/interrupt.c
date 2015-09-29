#include "interrupt.h"
#include "idt.h"
#include "isr.h"

void interrupt_init()
{
	idt_init();
}

void interrupt_handler(struct isr_parameter_registers regs)
{
	// yahooo
}

