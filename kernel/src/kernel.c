#include "cpu.h"
#include "log.h"
#include "irq.h"
#include "multiboot.h"
#include <startc/i386.h>
#include <startc/entry.h>
#include <sys/entry.h>
#include "panic.h"
#include "uart.h"
#include "memory.h"
#include "stream.h"

void _isr_cpu(unsigned code, struct _cpu_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n", code, regs->eip);
}

// These are defined in sys/errno.h as const, but we'll initialize them here.
int _stdin_id;
int _stdout_id;
int _stderr_id;

// Libstartc runtime calls this entrypoint function.
void _startc()
{
	assert(_multiboot);
	_memory_init(_multiboot);
	_stdin_id = _uart_open(&COM1);
	_stdout_id = _uart_open(&COM2);
	_stderr_id = _log_open();
	// Configure the IRQ table and enable interrupts.
	_irq_init();
	_sti();
	// Get the command line, if the bootloader gave us one.
	const char *cmdline = "";
	if (_multiboot->flags & 1<<2) {
		cmdline = (const char*)_multiboot->cmdline;
	}
	// Jump into the application entrypoint and let it do its thing.
	_main(cmdline);
	// Never return from _startc.
	while (1) _hlt();
}

