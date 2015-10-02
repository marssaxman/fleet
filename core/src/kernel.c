#include "cpu.h"
#include "log.h"
#include "interrupt.h"
#include "pcibus.h"
#include "gdt.h"
#include "multiboot.h"
#include "panic.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

static void check_multiboot(struct multiboot_info *info)
{
	// The bootloader has told us how much memory we have and what our
	// command line arguments were.
	_log(MULTIBOOT, "Multiboot structure from 0x%d to 0x%d\n",
			(int)info, (int)info + sizeof(struct multiboot_info));
	_log(MULTIBOOT, "  ->flags: 0x%d\n", info->flags);
	if (info->flags & 1<<0) {
		_log(MULTIBOOT, "  ->mem_upper: 0x%d\n", info->mem_upper);
	}
	if (info->flags & 1<<2) {
		_log(MULTIBOOT, "  ->cmdline: 0x%d\n", info->cmdline);
		_log(MULTIBOOT, "    %s\n", (const char*)info->cmdline);
	}
}

// Main entrypoint invoked by the _start function.
void _kernel(unsigned magic, struct multiboot_info *info)
{
	// Set up the logging port first, so we can stream debug information.
	_log_init();
	_log_print("\nfleet kernel status log\n");
	if (magic != 0x2BADB002) {
		_panic("Bad boot magic: 0x%d (should be 0x2BADBOO2)\n", magic);
	}
	check_multiboot(info);
	_gdt_init();
	_interrupt_init();
	_pcibus_init();
	_sti();
	main();
	while (1) {}
}

