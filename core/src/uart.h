#ifndef _UART_H
#define _UART_H

#include <string.h>

// Primitive, low-speed legacy serial interfaces, aka COM ports.

void com1_init();
size_t com1_write(const char *buf, size_t bytes);
size_t com1_read(char *buf, size_t capacity);

#endif //_UART_H
