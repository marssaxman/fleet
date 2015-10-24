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

static void check_multiboot(struct multiboot_info *info)
{
	// The bootloader has told us how much memory we have and what our
	// command line arguments were.
	_log(MULTIBOOT, "Multiboot structure from 0x%d to 0x%d\n",
			(int)info, (int)info + sizeof(struct multiboot_info));
	_log(MULTIBOOT, "  ->flags: 0x%d\n", info->flags);
	if (info->flags & 1<<2) {
		_log(MULTIBOOT, "  ->cmdline: 0x%d\n", info->cmdline);
		_log(MULTIBOOT, "    %s\n", (const char*)info->cmdline);
	}
}

void _isr_cpu(unsigned code, struct _cpu_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n", code, regs->eip);
}

// Libstartc runtime calls this entrypoint function.
void _startc()
{
	_log_print("\nfleet kernel status log\n");
	// Use our boot info to set up a memory map and find our configuration.
	if (!_multiboot) _panic("Bad boot: no multiboot header\n");
	// Configure the memory and interrupt systems.
	_memory_init(_multiboot);
	_irq_init();
	_sti();
	const char *cmdline = "";
	if (_multiboot->flags & 1<<2) {
		cmdline = (const char*)_multiboot->cmdline;
	}
	// Jump into the application entrypoint and let it do its thing.
	_main(cmdline);
	// Never return from _startc.
	while (1) _hlt();
}

