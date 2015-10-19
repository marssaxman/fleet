#include "debug-write.h"

// Use the port E9 hack to write data to the emulator's debug console.

void debug_write(const char *msg)
{
	while (*msg) {
		__asm__("outb %%al,%%dx;": :"d"(0xE9), "a"(*msg++));
	}
}

void debug_putc(char ch)
{
	__asm__("outb %%al,%%dx;": :"d"(0xE9), "a"(ch));
}

void debug_print_buffer(const void *buf, int bytes)
{
	static const char digits[] = "0123456789ABCDEF";
	const char *src = (const char*)buf;
	while (bytes--) {
		char ch = *src++;
		if (ch > 0x1F && ch < 0x7F) {
			debug_putc(ch);
		} else {
			debug_putc('\\');
			debug_putc('x');
			debug_putc(digits[ch & 0x0F]);
			debug_putc(digits[(ch >> 4) & 0x0F]);
		}
	}
}

