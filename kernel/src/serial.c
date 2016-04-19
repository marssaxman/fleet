// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include <stdint.h>
#include <stdbool.h>

struct portevt {
	struct task task;
	unsigned port;
};

static struct portevt txclearevt[4];
static struct portevt rxreadyevt[4];

static void txclear(struct task *t) {
	struct portevt *p = ring_item_struct(t, struct portevt, task);
	_kprintf("Port %i TX buffer cleared\n", p->port);
}

static void rxready(struct task *t) {
	struct portevt *p = ring_item_struct(t, struct portevt, task);
	_kprintf("Port %i RX buffer ready\n", p->port);
}

void _uart_line_status(unsigned port, unsigned LSR) {
}

void _uart_modem_status(unsigned port, unsigned MSR) {
}

void mic_check() {
	txclearevt[0].port = 0;
	txclearevt[0].task.method = txclear;
	void *buffer = "Hello, world!\r\n";
	_uart_transmit(0, buffer, 15, &txclearevt[0].task);
}
