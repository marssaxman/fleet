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
	struct transfer_queue tx, rx;
} com[4];

static void tq_push(struct transfer_queue *q, struct stream_transfer *t) {
	ring_push(&q->pending, &t->link);
}

static void tq_pull(struct transfer_queue *q, struct uart_buffer *next) {
	if (q->current) {
		post(&q->current->event);
	}
	struct ring_item *link = ring_pull(&q->pending);
	if (link) {
		q->current = container_of(link, struct stream_transfer, link);
		next->ptr = q->current->request.buffer;
		next->length = q->current->request.size;
	} else {
		q->current = 0;
	}
}

void _serial_init() {
	_uart_init();
	for (unsigned i = 0; i < 4; ++i) {
		ring_init(&com[i].tx.pending);
		ring_init(&com[i].rx.pending);
	}
}

unsigned _serial_transmit(stream_socket s, struct stream_transfer *t) {
	tq_push(&com[s].tx, t);
	_uart_transmit(s);
	return 0;
}

unsigned _serial_receive(stream_socket s, struct stream_transfer *t) {
	tq_push(&com[s].rx, t);
	_uart_receive(s);
	return 0;
}

void _uart_isr_thre(uart_id port, struct uart_buffer *next) {
	tq_pull(&com[port].tx, next);
}

void _uart_isr_rbr(uart_id port, struct uart_buffer *next) {
	tq_pull(&com[port].rx, next);
}

void _uart_line_status(uart_id port, uint8_t LSR) {
}

void _uart_modem_status(uart_id port, uint8_t MSR) {
}


