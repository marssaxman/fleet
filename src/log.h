// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

// The log device is the 0xE9 hypervisor console. It is also used as stderr.

int _log_open();

void _log_putch(char ch);
void _log_print(const char *str);
void _log_puts(const char *str);
void _log_vprintf(const char *format, va_list args);
void _log_printf(const char *format,...);

// Enable or disable different categories of logging.

enum _log_categories
{
	_LOG_INFO = 1,
	_LOG_PCIPROBE = 0,
	_LOG_MULTIBOOT = 0,
	_LOG_ISR = 0,
	_LOG_WORKQUEUE = 0,
	_LOG_MEMORY = 1,
};

#define _log(category, format, args...) \
	do { if (_LOG_##category) _log_printf(format, ## args); } while (0)

#endif // LOG_H
