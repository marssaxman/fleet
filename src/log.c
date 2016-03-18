// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdint.h>
#include <stdarg.h>
#include "log.h"
#include "socket.h"
#include "console.h"
#include <errno.h>

// Write log messages to the hypervisor's debug console.

static int log_write(void*, const void *buf, unsigned size);
static int log_stream_id;
static struct iops log_ops = {
	.write = log_write,
};

int _log_open()
{
	if (log_stream_id) return -EISCONN;
	return log_stream_id = open(0, &log_ops);
}

static int log_write(void *ref, const void *buf, unsigned size) {
	_console_write(buf, size);
	return size;
}

static void puthex(char ch)
{
	static const char digits[] = "0123456789ABCDEF";
	_console_putc(digits[(ch >> 4) & 0x0F]);
	_console_putc(digits[ch & 0x0F]);
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
