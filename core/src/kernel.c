#include "cpu.h"
#include "log.h"
#include "exception.h"
#include "idt.h"
#include "irq.h"
#include "multiboot.h"
#include "panic.h"
#include "events.h"
#include "uart.h"

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
		_log(MULTIBOOT, "  ->mem_lower: 0x%d\n", info->mem_lower * 1024);
		_log(MULTIBOOT, "  ->mem_upper: 0x%d\n", info->mem_upper * 1024);
	}
	if (info->flags & 1<<2) {
		_log(MULTIBOOT, "  ->cmdline: 0x%d\n", info->cmdline);
		_log(MULTIBOOT, "    %s\n", (const char*)info->cmdline);
	}
	if (info->flags & 1<<6) {
		_log(MULTIBOOT, "  ->mmap_length: 0x%d\n", info->mmap_length);
		_log(MULTIBOOT, "  ->mmap_addr: 0x%d\n", info->mmap_addr);
		multiboot_info_t *mbi = info;
		memory_map_t *mmap;
		for (mmap = (memory_map_t *) mbi->mmap_addr;
			(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
			mmap = (memory_map_t *) ((unsigned long) mmap
				+ mmap->size + sizeof (mmap->size))) {
		_log_printf (" size = 0x%x, base_addr = 0x%x%x,"
		" length = 0x%x%x, type = 0x%x\n",
		(unsigned) mmap->size,
		(unsigned) mmap->base_addr_high,
		(unsigned) mmap->base_addr_low,
		(unsigned) mmap->length_high,
		(unsigned) mmap->length_low,
		(unsigned) mmap->type);
		}
	}
}

static void _tx_clear(void *value)
{
	_log_print("TX_CLEAR on COM1");
}
static struct work_item com1_tx_clear;

// Main entrypoint invoked by the _start function.
void _kernel(unsigned magic, struct multiboot_info *info)
{
	_log_print("\nfleet kernel status log\n");
	// Use our boot info to set up a memory map and find our configuration.
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		_panic("Bad boot magic: 0x%d (should be 0x2BADBOO2)\n", magic);
	}
	check_multiboot(info);
	// Configure the memory and interrupt systems.
	_idt_init();
	_exception_init();
	_irq_init();
	_sti();
	// Jump into the application entrypoint and let it do its thing.
	work_item_init(&com1_tx_clear, _tx_clear, &COM1);
	COM1.events.tx_clear = &com1_tx_clear;
	uart_open(&COM1);
	uart_write(&COM1, "THIS IS A TEST YO YO YO DUDE MAN", 22);
	main();
	// The app is done, so now we sleep and process interrupts forever.
	while (1) {
		poll_events();
		_hlt();
	}
}

