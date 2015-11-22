#include <stdio.h>
#include <stdarg.h>

int fprintf(FILE *stream, const char *format, ...)
{
	va_list va;
    va_start(va, format);
	int ret = vfprintf(stream, format, va);
	va_end(va);
	return ret;
}

