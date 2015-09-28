#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vga.h>
#include <interrupt.h>

void main(void)
{
	// Set up the terminal display
	terminal_initialize();
	// Configure the interrupt table
	interrupt_init();
	// Trigger some interrupts to see what happens
//	__asm__("int $0x3");
//	__asm__("int $0x4");
}

