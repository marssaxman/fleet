#ifndef _UART_H
#define _UART_H

#include <string.h>
#include "task.h"

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct uart {
	unsigned port;
	unsigned irq;
	struct _task listen;
	struct uart_events {
		struct _task *tx_clear;
		struct _task *rx_ready;
	} events;
};
void uart_init(struct uart*, struct uart_events *proc);
size_t uart_write(struct uart*, const char *buf, size_t bytes);
size_t uart_read(struct uart*, char *buf, size_t capacity);

extern struct uart COM1;
extern struct uart COM2;
extern struct uart COM3;
extern struct uart COM4;

#endif //_UART_H
