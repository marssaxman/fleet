// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include "serial.h"
#include "interrupt.h"
#include <stdint.h>
#include <stdbool.h>

struct ioqueue {
	struct stream_transfer *current;
	struct ring_list pending;
};

static struct serial {
	struct irq_action signal;
	struct uart_state state;
	struct ioqueue tx, rx;
} com[4];

static void ioqueue_init(struct ioqueue *q) {
	q->current = 0;
	ring_init(&q->pending);
}

static void ioqueue_push(struct ioqueue *q, struct stream_transfer *t) {
	ring_push(&q->pending, &t->link);
}

static void ioqueue_pull(struct ioqueue *q, struct iovec *next) {
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

static void _serial_isr(struct irq_action *context) {
	struct serial *port = container_of(context, struct serial, signal);
	_uart_service(&port->state);
}

void _serial_init() {
	// Traditional IO port addresses for the standard PC UARTs, COM1-COM4.
	static const uint16_t com_addrs[4] = {0x03F8, 0x02F8, 0x03E8, 0x02F8};
	static const uint8_t com_irqs[4] = {4, 3, 4, 3};
	for (unsigned i = 0; i < 4; ++i) {
		// Verify that we can communicate with a UART at this address.
		if (0 != _uart_probe(com_addrs[i])) continue;
		struct serial *port = &com[i];
		port->state.addr = com_addrs[i];
		ioqueue_init(&port->tx);
		ioqueue_init(&port->rx);
		port->signal.isr = _serial_isr;
		_irq_attach(com_irqs[i], &port->signal);
		_uart_open(port->state.addr);
	}
}

unsigned _serial_transmit(stream_socket s, struct stream_transfer *t) {
	ioqueue_push(&com[s].tx, t);
	_uart_tx_start(com[s].state.addr);
	return 0;
}

unsigned _serial_receive(stream_socket s, struct stream_transfer *t) {
	ioqueue_push(&com[s].rx, t);
	_uart_rx_start(com[s].state.addr);
	return 0;
}

void _uart_isr_thre(struct uart_state *dev) {
	struct serial *port = container_of(dev, struct serial, state);
	ioqueue_pull(&port->tx, &dev->tx);
}

void _uart_isr_rbr(struct uart_state *dev) {
	struct serial *port = container_of(dev, struct serial, state);
	ioqueue_pull(&port->rx, &dev->rx);
}

void _uart_isr_lsi(struct uart_state *dev, uint8_t LSR) {
	struct serial *port = container_of(dev, struct serial, state);
	int index = (port - com) / sizeof(struct serial);
	_kprintf("LSI on COM%d: LSR=%x\n", index, LSR);
}

void _uart_isr_msi(struct uart_state *dev, uint8_t MSR) {
	struct serial *port = container_of(dev, struct serial, state);
	int index = (port - com) / sizeof(struct serial);
	_kprintf("MSI on COM%d: MSR=%x\n", index, MSR);
}


