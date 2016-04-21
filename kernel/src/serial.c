// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include "serial.h"
#include <stdint.h>
#include <stdbool.h>

struct transfer_queue {
	struct stream_transfer *current;
	struct ring_list pending;
};

static struct serial_socket_data {
	struct uart_state *state;
	struct transfer_queue tx, rx;
} com[4];

static void tq_push(struct transfer_queue *q, struct stream_transfer *t) {
	ring_push(&q->pending, &t->link);
}

static void tq_pull(struct transfer_queue *q, struct iovec *next) {
	if (q->current) {
		post(&q->current->signal);
	}
	struct ring_item *link = ring_pull(&q->pending);
	if (link) {
		q->current = container_of(link, struct stream_transfer, link);
		next->base = q->current->request.base;
		next->size = q->current->request.size;
	} else {
		q->current = 0;
	}
}

void _serial_init() {
	// Traditional IO port addresses for the standard PC UARTs, COM1-COM4.
	static const uint16_t com_addrs[4] = {0x03F8, 0x02F8, 0x03E8, 0x02F8};
	for (unsigned i = 0; i < 4; ++i) {
		struct serial_socket_data *data = &com[i];
		data->state = &_uart_state[i];
		if (0 != _uart_probe(data->state, com_addrs[i])) {
			continue;
		}
		ring_init(&data->tx.pending);
		ring_init(&data->rx.pending);
		_uart_open(data->state);
	}
	// enable IRQ3 and IRQ4
	__asm__("inb $0x0021, %al");
	__asm__("andb $0xE7, %al");
	__asm__("outb %al, $0x0021");
}

unsigned _serial_transmit(stream_socket s, struct stream_transfer *t) {
	tq_push(&com[s].tx, t);
	_uart_tx_start(com[s].state);
	return 0;
}

unsigned _serial_receive(stream_socket s, struct stream_transfer *t) {
	tq_push(&com[s].rx, t);
	_uart_rx_start(com[s].state);
	return 0;
}

void _uart_isr_thre(struct uart_state *port) {
	tq_pull(&com[port->index].tx, &port->tx);
}

void _uart_isr_rbr(struct uart_state *port) {
	tq_pull(&com[port->index].rx, &port->rx);
}

void _uart_isr_lsi(struct uart_state *port, uint8_t LSR) {
	_kprintf("LSI on COM%d: LSR=%x\n", port->index, LSR);
}

void _uart_isr_msi(struct uart_state *port, uint8_t MSR) {
	_kprintf("MSI on COM%d: MSR=%x\n", port->index, MSR);
}


