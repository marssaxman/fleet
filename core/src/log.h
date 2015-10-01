#ifndef _LOG_H
#define _LOG_H

#include <stdarg.h>
#include <stdbool.h>

// Write kernel status information to the legacy COM1 port.

void _log_init();
void _log_putch(char ch);
void _log_print(const char *str);
void _log_puts(const char *str);
void _log_vprintf(const char *format, va_list args);
void _log_printf(const char *format,...);

// Enable or disable different categories of logging.

enum _log_categories
{
	_LOG_PCIPROBE = false,
	_LOG_MULTIBOOT = true,
	_LOG_ISR = true,
};

#define _log(category, format, args...) \
	do { if (_LOG_##category) _log_printf(format, ## args); } while (0)

#endif //_LOG_H