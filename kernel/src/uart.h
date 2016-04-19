// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <fleet/system.h>

void _uart_init();

typedef uint8_t uart_id;

// If the port is not already transmitting or receiving, wake it up and start
// its interrupt-driven state machine.
void _uart_transmit(uart_id port);
void _uart_receive(uart_id port);

struct uart_buffer {
	void *ptr;
	size_t length;
};

// The port has cleared its transmit buffer. Populate its buffer struct with
// a new ptr and length, or the transmitter will go to sleep.
extern void _uart_isr_thre(uart_id port, struct uart_buffer *next);

// The port has filled its receive buffer. Give it a new buffer, or it will
// drop DTR and the receiver will sleep.
extern void _uart_isr_rbr(uart_id port, struct uart_buffer *next);

extern void _uart_line_status(uart_id port, uint8_t LSR);
extern void _uart_modem_status(uart_id port, uint8_t MSR);

#endif //UART_H

