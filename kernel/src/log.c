#include <stdint.h>
#include <stdarg.h>
#include "log.h"
#include "cpu.h"
#include "socket.h"
#include <sys/errno.h>

// Write log messages to the hypervisor's debug console.

static int log_transmit(void*, struct transfer*);
static int log_stream_id;
static struct iops log_ops = {
	.transmit = log_transmit,
};

int _log_open()
{
	if (log_stream_id) return EISCONN;
	return log_stream_id = _open(0, &log_ops);
}

static int log_transmit(void *ref, struct transfer *xfer)
{
	const char *src = xfer->buffer;
	size_t bytes = xfer->length;
	while (bytes--) {
		_outb(0xE9, *src++);
	}
	xfer->completion(xfer, xfer->length);
}

void _log_putchar(char ch)
{
	_outb(0xE9, ch);
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

void _log_vprintf(const char *fmt, va_list args)
{
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
			case 'x':
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
}

void _log_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	_log_vprintf(fmt, args);
	va_end(args);
}
