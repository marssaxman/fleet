#include <stdio.h>
#include <stdarg.h>

int printf(const char *format, ...)
{
	va_list va;
    va_start(va, format);
	int ret = vfprintf(stdout, format, va);
	va_end(va);
	return ret;
}

