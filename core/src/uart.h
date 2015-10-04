#ifndef _UART_H
#define _UART_H

#include <string.h>
#include <workqueue.h>

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct uart {
	unsigned port;
	unsigned irq;
	struct work_item listen;
	// uart user supplies work items to run when events occur
	struct uart_events {
		struct work_item *tx_clear;
		struct work_item *rx_ready;
	} events;
};

void uart_init(struct uart*);
size_t uart_write(struct uart*, const char *buf, size_t bytes);
size_t uart_read(struct uart*, char *buf, size_t capacity);

extern struct uart COM1;
extern struct uart COM2;
extern struct uart COM3;
extern struct uart COM4;

#endif //_UART_H
