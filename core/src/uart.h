#ifndef _UART_H
#define _UART_H

#include <string.h>
#include "task.h"

// Primitive, low-speed legacy serial interfaces, aka COM ports.

// Events the port may raise 
struct com1_events {
	// Sent all the data: need more
	struct _task *tx_clear;
	// Data came in: you should grab it
	struct _task *rx_ready;
};

// Set up the port and connect it to your event handlers
void com1_init(struct com1_events *proc);

// Transfer data to the port, stopping when the source buffer ends or the port
// buffer fills up, returning the number of bytes transferred
size_t com1_write(const char *buf, size_t bytes);

// Transfer data in from the port, stopping when its buffer empties or the
// destination buffer fills up, returning the number of bytes transferred
size_t com1_read(char *buf, size_t capacity);

#endif //_UART_H
