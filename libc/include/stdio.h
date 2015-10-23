#ifndef _STDIO_H
#define _STDIO_H

#include <_va_list.h>
#include <stddef.h>

int sprintf(char *str, const char *format, ...);
int snprintf(char *s, size_t max, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int vsnprintf(char *s, size_t max, const char *format, va_list arg);

#endif //_STDIO_H

