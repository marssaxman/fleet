// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include <stdint.h>
#include <stdbool.h>

void _uart_tx_clear(unsigned port) {
	_kprintf("Port %i TX buffer cleared\n", port);
}

void _uart_rx_ready(unsigned port) {
	_kprintf("Port %i RX buffer ready\n", port);
}

void _uart_modem_status(unsigned port, unsigned state) {
}

void _uart_line_status(unsigned port, unsigned state) {
}

