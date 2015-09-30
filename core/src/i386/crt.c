#include <i386/isr.h>
#include <i386/idt.h>

// Somewhere out there, the user has implemented a main function.
extern void main(void);

static void brutal_log(const char *msg)
{
	static unsigned line = 0;
	uint16_t *terminal_buffer = (uint16_t*) 0xB8000;
	uint16_t *linebuf = &terminal_buffer[80 * line];
	for (unsigned i = 0; i < 80 && *msg; ++i) {
		linebuf[i] = 0x0F00 | *msg++;
	}
	line++;
	if (line >= 25) line = 0;
}

// The ISR module expects us to provide a master service routine.
void _isr(struct _isr_state *regs)
{
	brutal_log("the _isr was fired");
}

static void isr_init()
{
	static struct _idt_entry idt[256];
	for (unsigned i = 0; i < 256; ++i) {
		uint32_t stubaddr = (uint32_t)&_isr_stubs[i];
		idt[i].offset_low = stubaddr & 0x0000FFFF;
		idt[i].offset_high = (stubaddr >> 16) & 0x0000FFFF;
		idt[i].selector = 0x08; // the only code segment we use
		idt[i].flags = 0x8E;	// 32-bit interrupt gate
	}
	_idt_load(256, idt);
}

// Main entrypoint invoked by the _start function when the assembly bootstrap
// is ready to hand things over to the C world.
void _crt(void)
{
	isr_init();
	main();
}

