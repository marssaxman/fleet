// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

void _uart_init();
void _uart_transmit(unsigned port, const void *buf, unsigned len);
void _uart_receive(unsigned port, void *buf, unsigned max);

enum {
	UART_OVERFLOW_ERROR = 1,
	UART_PARITY_ERROR = 2,
	UART_FRAMING_ERROR = 4,
	UART_BREAK_INDICATOR = 8
};
extern void _uart_line_status(unsigned port, unsigned conditions);
extern void _uart_modem_status(unsigned port, unsigned conditions);
extern void _uart_tx_clear(unsigned port);
extern void _uart_rx_ready(unsigned port, unsigned len);

#endif //UART_H

