#ifndef _UART_H
#define _UART_H

#include <stddef.h>
#include "signal.h"

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct uart {
	unsigned port;
	unsigned irq;
	struct signal_action listen;
	// uart user supplies work items to run when events occur
	struct uart_events {
		struct signal tx_clear;
		struct signal rx_ready;
	} events;
};

void uart_open(struct uart*);
size_t uart_write(struct uart*, const char *buf, size_t bytes);
size_t uart_read(struct uart*, char *buf, size_t capacity);
void uart_close(struct uart*);

extern struct uart COM1;
extern struct uart COM2;
extern struct uart COM3;
extern struct uart COM4;

#endif //_UART_H