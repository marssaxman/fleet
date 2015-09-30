#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vga.h>

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


#if defined(__cplusplus)
extern "C"
#endif
void main(void) {
	/* Initialize terminal interface */
	terminal_initialize();

	/* Since there is no support for newlines in terminal_putchar
         * yet, '\n' will produce some VGA specific character instead.
         * This is normal.
         */
	terminal_writestring("Hello, world!\n");
}

