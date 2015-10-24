#ifndef UART_H
#define UART_H

#include <stddef.h>

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct uart;

extern struct uart COM1;
extern struct uart COM2;
extern struct uart COM3;
extern struct uart COM4;

int _uart_open(struct uart*);


#endif // UART_H
