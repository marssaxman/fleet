// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdint.h>
#include <stdarg.h>
#include "debug.h"
#include "console.h"

// Write log messages to the hypervisor's debug console.

static void puthex(char ch) {
	static const char digits[] = "0123456789ABCDEF";
	_console_putc(digits[(ch >> 4) & 0x0F]);
	_console_putc(digits[ch & 0x0F]);
}

static const char *handle_sizemod(const char *fmt, int *sizemod) {
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

void _kvprintf(const char *fmt, va_list args) {
	while (*fmt) {
		// handle the simple case first: the character stands for itself,
		// and a doubled % escapes itself.
		if (*fmt != '%' || *++fmt == '%') {
			_console_putc(*fmt++);
			continue;
		}

		// If it's an actual escape, look for a numeric size prefix.
		int sizemod = 0;
		fmt = handle_sizemod(fmt, &sizemod);
		// Now see what the actual type code might happen to be.
		switch (*fmt) {
			case 's': {
				char *str = va_arg(args, char*);
				while (*str) _console_putc(*str++);
				fmt++;
			} break;
			case 'c': {
				_console_putc(va_arg(args, int));
				fmt++;
			} break;
			case 'd':
			case 'i': {
				int value = va_arg(args, int);
				if (value < 0) {
					_console_putc('-');
					value *= -1;
				}
				char buf[16];
				char *pos = &buf[16];
				do {
					*--pos = (value % 10) + '0';
					value /= 10;
				} while (value > 0);
				_console_write(pos, &buf[16]-pos);
				fmt++;
			} break;
			case 'x': {
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

void _kprintf(const char *fmt, ...) {
	va_list args;
	va_start(args,fmt);
	_kvprintf(fmt, args);
	va_end(args);
}

void _panic(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	_kprintf("panic: ");
	_kvprintf(msg, args);
	while (1) {}
}

void _kassertfail(const char *file, int line, const char *cond) {
	_panic("assertion failed at %s:%d: %s\n", file, line, cond);
}

