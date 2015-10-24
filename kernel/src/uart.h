#ifndef UART_H
#define UART_H

#include <stddef.h>

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct uart {
	unsigned port;
	unsigned irq;
};

void _uart_open(struct uart*);
size_t _uart_write(struct uart*, const char *buf, size_t bytes);
size_t _uart_read(struct uart*, char *buf, size_t capacity);
void _uart_close(struct uart*);

extern struct uart COM1;
extern struct uart COM2;
extern struct uart COM3;
extern struct uart COM4;

#endif // UART_H
