#include <i386/isr.h>
#include <i386/idt.h>
#include <i386/cpu.h>
#include "sys/syslog.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

// The ISR module expects us to provide a master service routine.
void _isr(struct _isr_state *regs)
{
	syslog_puts("the _isr was fired");
	_hlt();
	while (1) {}
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

// Main entrypoint invoked by the _start function when the assembly bootstrap
// is ready to hand things over to the C world.
void _crt(void)
{
	syslog_init();
	syslog_printf("Hello, fleet %i %i\n", 42, 0x0F0F);
	idt_init();
	_sti();
	main();
	while (1) {}
}

