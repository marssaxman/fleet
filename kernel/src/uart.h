// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <fleet/event.h>
#include <fleet/io.h>

// Low-level UART control functions which are more conveniently implemented in
// assembly language, supporting the actual device driver in serial.c.

struct uart_state {
	uint16_t addr;
	uint8_t index;
	uint8_t irq;
	struct iovec tx;
	struct iovec rx;
};

int _uart_probe(struct uart_state *port);
void _uart_open(struct uart_state*);
void _uart_close(struct uart_state*);
void _uart_tx_start(struct uart_state*);
void _uart_tx_stop(struct uart_state*);
void _uart_rx_start(struct uart_state*);
void _uart_rx_stop(struct uart_state*);
void _uart_service(struct uart_state*);

extern void _uart_isr_thre(struct uart_state *port);
extern void _uart_isr_rbr(struct uart_state *port);
extern void _uart_isr_lsi(struct uart_state *port, uint8_t LSR);
extern void _uart_isr_msi(struct uart_state *port, uint8_t MSR);

#endif //UART_H

