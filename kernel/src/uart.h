// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <sys/io.h>

void _uart_init();

void _uart_transmit(unsigned port, struct io_buffer);
void _uart_receive(unsigned port, struct io_buffer);

enum {
	UART_OVERFLOW_ERROR = 0x01,
	UART_PARITY_ERROR = 0x02,
	UART_FRAMING_ERROR = 0x04,
	UART_BREAK_INDICATOR = 0x08,
	UART_CLEAR_TO_SEND = 0x10,
	UART_DATA_SET_READY = 0x20,
	UART_RING_INDICATOR = 0x40,
	UART_CARRIER_DETECT = 0x80,
};
extern void _uart_line_status(unsigned port, unsigned conditions);
extern void _uart_modem_status(unsigned port, unsigned conditions);
extern void _uart_tx_clear(unsigned port);
extern void _uart_rx_ready(unsigned port);

#endif //UART_H

