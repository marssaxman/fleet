#include <stdint.h>
#include <stdarg.h>
#include "log.h"
#include "cpu.h"

// Stream log information to COM1 for use by host debugging.

static uint16_t COM1 = 0x03F8;

void _log_init()
{
	_outb(COM1 + 3, 0x80); // enable DLAB
	_outb(COM1 + 0, 0x01); // set divisor to 1 (115200baud)
	_outb(COM1 + 1, 0x00); // hi byte of divisor
	_outb(COM1 + 3, 0x03); // 8N1 
	_outb(COM1 + 2, 0xC7); // enable FIFO
}

void _log_putchar(char ch)
{
	// Wait until the port is ready to receive
	while (0 == (_inb(COM1 + 5) & 0x20)) { /*wait*/ }
	// Push another byte into its buffer
	_outb(COM1, ch);
}

void _log_print(const char *str)
{
	while (*str) {
		_log_putchar(*str++);
	}
}

void _log_puts(const char *str)
{
	_log_print(str);
	_log_putchar('\n');
}

static void puthex(char ch)
{
	static const char digits[] = "0123456789ABCDEF";
	_log_putchar(digits[(ch >> 4) & 0x0F]);
	_log_putchar(digits[ch & 0x0F]);
}

static const char *handle_sizemod(const char *fmt, int *sizemod)
{
	while (*fmt) {
		switch (*fmt) {
			case 'l': (*sizemod)++; break;
			case 'h': (*sizemod)--; break;
			default: return fmt;
		}
		fmt++;
	}
	return fmt;
}

void _log_printf(const char *fmt,...)
{
	va_list args;
	va_start(args,fmt);
	while (*fmt) {
		// handle the simple case first: the character stands for itself,
		// and a doubled % escapes itself.
		if (*fmt != '%' || *++fmt == '%') {
			_log_putchar(*fmt++);
			continue;
		}

		// If it's an actual escape, look for a numeric size prefix.
		int sizemod = 0;
		fmt = handle_sizemod(fmt, &sizemod);
		// Now see what the actual type code might happen to be.
		switch (*fmt) {
			case 's': {
				char *str = va_arg(args, char*);
				_log_print(str);
				fmt++;
			} break;
			case 'c': {
				char str[2] = {va_arg(args, int), '\0'};
				_log_print(str);
				fmt++;
			} break;
			case 'd':
			case 'i': {
				int value = va_arg(args, int);
				if (sizemod >= 0) {
					puthex((value >> 24) & 0x00FF);
					puthex((value >> 16) & 0x00FF);
				}
				if (sizemod >= -1) {
					puthex((value >> 8) & 0x00FF);
				}
				puthex(value & 0x00FF);
				fmt++;
			} break;
		}
	}
	va_end(args);
}

