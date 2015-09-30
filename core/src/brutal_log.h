#ifndef _BRUTAL_LOG_H
#define _BRUTAL_LOG_H

#include <stdint.h>

void brutal_putch(char ch);
void brutal_newline();
void brutal_puts(const char *str);
void brutal_putln(const char *str);
void brutal_showhex(const void *src, uint16_t digits);

#endif //_BRUTAL_LOG_H
