// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include <stdint.h>
#include <stdbool.h>

// scratch space for functions the assembly code in uart.s can invoke to help
// debug the serial port driver.

void _uart_modem_status(unsigned port, unsigned msr) {
	_kprintf("\nPort %i MSR:%x\n", port, msr);
}

void _uart_line_status(unsigned port, unsigned lsr) {
	_kprintf("\nPort %i LSR: %s %s %s %s\n", port,
		(lsr & UART_OVERFLOW_ERROR)? "OE": "--",
		(lsr & UART_PARITY_ERROR)? "PE": "--",
		(lsr & UART_FRAMING_ERROR)? "FE": "--",
		(lsr & UART_BREAK_INDICATOR)? "BI": "--");
}

void _uart_tx_clear(unsigned port) {
	unsigned num = _uart_get_tx_count(port);
	_kprintf("Port %i TX cleared %i bytes\n", port, num);
}

void _uart_rx_ready(unsigned port) {
	unsigned num = _uart_get_rx_count(port);
	_kprintf("Port %i RX ready with %i bytes\n", port, num);
}
