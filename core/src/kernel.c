#include "cpu.h"
#include "log.h"
#include "irq.h"
#include "multiboot.h"
#include <startc/i386.h>
#include "panic.h"
#include "events.h"
#include "uart.h"
#include "memory.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

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

static void _tx_clear(void *value)
{
	_log_print("TX_CLEAR on COM1");
}
static struct signal_action com1_tx_clear;

void _isr_cpu(unsigned code, struct _cpu_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n", code, regs->eip);
}

// Libstartc runtime calls this entrypoint function.
void _startc(unsigned magic, struct multiboot_info *info)
{
	_log_print("\nfleet kernel status log\n");
	// Use our boot info to set up a memory map and find our configuration.
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		_panic("Bad boot magic: 0x%d (should be 0x2BADBOO2)\n", magic);
	}
	check_multiboot(info);
	// Configure the memory and interrupt systems.
	_memory_init(info);
	_irq_init();
	_sti();
	// Jump into the application entrypoint and let it do its thing.
	action_init(&com1_tx_clear, _tx_clear, &COM1);
	signal_listen(&COM1.events.tx_clear, &com1_tx_clear);
	uart_open(&COM1);
	uart_write(&COM1, "THIS IS A TEST YO YO YO DUDE MAN", 22);
	main();
	// The app is done, so now we sleep and process interrupts forever.
	while (1) {
		poll_events();
		_hlt();
	}
}

