#include "interrupt.h"
#include <i386/isr.h>
#include <i386/cpu.h>
#include <i386/idt.h>
#include <sys/log.h>
#include "pic.h"
#include "panic.h"

// The ISR module expects us to provide a master service routine.
void _isr(struct _isr_state *regs)
{
	_log_printf("_isr:\n\tedi = %d, esi = %d, ebp = %d, esp = %d\n"
			"\tebx = %d, edx = %d, ecx = %d, eax = %d\n"
			"\tinterrupt_number = %d\n"
			"\terror_code = %d\n"
			"\teip = %d, cs = %d, eflags = %d\n",
			regs->edi, regs->esi, regs->ebp, regs->esp,
			regs->ebx, regs->edx, regs->ecx, regs->eax,
			regs->interrupt_number,
			regs->error_code,
			regs->eip, regs->cs, regs->eflags);
	panic("unhandled interrupt");
}

static void idt_init()
{
	static struct _idt_entry idt[256];
	for (unsigned i = 0; i < 256; ++i) {
		uint32_t stubaddr = (uint32_t)&_isr_stubs[i];
		idt[i].offset_low = stubaddr & 0x0000FFFF;
		idt[i].offset_high = (stubaddr >> 16) & 0x0000FFFF;
		idt[i].selector = 0x08; // the only code segment we use
		idt[i].flags = 0x8E;	// 32-bit interrupt gate
	}
	static struct _idt_descriptor idtptr;
	idtptr.size = sizeof(struct _idt_entry) * 256 - 1;
	idtptr.offset = (uint32_t)idt;
	_idt_load(&idtptr);
}

void _interrupt_init()
{
	idt_init();
	_pic_init();
}
