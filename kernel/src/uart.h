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

int _uart_probe(uint16_t addr);
void _uart_open(uint16_t addr);
void _uart_close(uint16_t addr);

enum uart_condition {
	UART_NONE = 0,
	UART_THRI,
	UART_RBRI,
	UART_LSI,
	UART_MSI,
};
enum uart_condition _uart_poll(uint16_t addr);
uint8_t _uart_line_status(uint16_t addr);
uint8_t _uart_modem_status(uint16_t addr);

void _uart_tx_start(uint16_t addr);
void _uart_tx_stop(uint16_t addr);
void _uart_tx_service(uint16_t addr, struct iovec *buf);

void _uart_rx_start(uint16_t addr);
void _uart_rx_stop(uint16_t addr);
void _uart_rx_service(uint16_t addr, struct iovec *buf);

struct uart_state {
	uint16_t addr;
	uint16_t _unused;
	struct iovec tx;
	struct iovec rx;
};
void _uart_service(struct uart_state*);

extern void _uart_isr_thre(struct uart_state *port);
extern void _uart_isr_rbr(struct uart_state *port);
extern void _uart_isr_lsi(struct uart_state *port, uint8_t LSR);
extern void _uart_isr_msi(struct uart_state *port, uint8_t MSR);

#endif //UART_H

