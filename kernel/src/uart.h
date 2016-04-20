// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <fleet/system.h>
#include <fleet/io.h>

struct uart_state {
	uint16_t addr;
	uint8_t index;
	uint8_t flags;
	struct iovec tx;
	struct iovec rx;
};
extern struct uart_state _uart_state[4];

void _uart_init();

// If the port is not already transmitting or receiving, wake it up and start
// its interrupt-driven state machine.
void _uart_transmit(struct uart_state *port);
void _uart_receive(struct uart_state *port);

extern void _uart_isr_thre(struct uart_state *port);
extern void _uart_isr_rbr(struct uart_state *port);
extern void _uart_isr_lsi(struct uart_state *port, uint8_t LSR);
extern void _uart_isr_msi(struct uart_state *port, uint8_t MSR);

#endif //UART_H

