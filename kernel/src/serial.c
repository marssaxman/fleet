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

struct channel {
	struct stream_transfer *current;
	struct ring_list pending;
	struct iovec buffer;
};

static struct serial {
	struct irq_action signal;
	struct channel tx, rx;
	uint16_t addr;
} com[4];

enum LSR_FLAGS {
	LSR_DR = 0x01, // Data ready
	LSR_OE = 0x02, // Overrun error
	LSR_PE = 0x04, // Parity error
	LSR_FE = 0x08, // Frame error
	LSR_BI = 0x10, // Break indicator
	LSR_THRE = 0x20, // Transmit holding register empty
	LSR_TEMT = 0x40, // Transmitter empty
	LSR_RXFE = 0x80, // Error in receiver FIFO
};

enum MSR_FLAGS {
	MSR_DCTS = 0x01, // CTS change
	MSR_DDSR = 0x02, // DSR change
	MSR_TERI = 0x04, // Ring indicator ended
	MSR_DDCD = 0x08, // DCD change
	MSR_CTS = 0x10, // Clear to send
	MSR_DSR = 0x20, // Data set ready (present)
	MSR_RI = 0x40, // Ring indicator
	MSR_DCD = 0x80, // Data carrier detected
};

static void channel_init(struct channel *q) {
	q->current = 0;
	ring_init(&q->pending);
	q->buffer.base = 0;
	q->buffer.size = 0;
}

static void channel_push(struct channel *q, struct stream_transfer *t) {
	ring_push(&q->pending, &t->link);
}

static void channel_pull(struct channel *q) {
	if (q->current) {
		post(&q->current->signal);
	}
	struct ring_item *link = ring_pull(&q->pending);
	if (link) {
		q->current = container_of(link, struct stream_transfer, link);
		q->buffer = q->current->request;
	} else {
		q->current = 0;
		q->buffer.base = 0;
		q->buffer.size = 0;
	}
}

static void transmit_clear(struct serial *port) {
	for(;;) {
		_uart_tx_service(port->addr, &port->tx.buffer);
		if (0 != port->tx.buffer.size) return;
		channel_pull(&port->tx);
		if (0 == port->tx.current) {
			_uart_tx_stop(port->addr);
			return;
		}
	}
}

static void receive_ready(struct serial *port) {
	for (;;) {
		_uart_rx_service(port->addr, &port->rx.buffer);
		if (0 != port->rx.buffer.size) return;
		channel_pull(&port->rx);
		if (0 == port->rx.current) {
			_uart_rx_stop(port->addr);
			return;
		}
	}
}

static void line_event(struct serial *port) {
	int index = (port - com) / sizeof(struct serial);
	unsigned LSR = _uart_line_status(port->addr);
	_kprintf("LSI on COM%d: LSR=%x\n", index, LSR);
}

static void modem_event(struct serial *port) {
	int index = (port - com) / sizeof(struct serial);
	unsigned MSR = _uart_modem_status(port->addr);
	_kprintf("MSI on COM%d: MSR=%x\n", index, MSR);
}

static void service(struct irq_action *context) {
	struct serial *port = container_of(context, struct serial, signal);
	for (;;) switch (_uart_poll(port->addr)) {
		case UART_NONE: return;
		case UART_THRI: transmit_clear(port); continue;
		case UART_RBRI: receive_ready(port); continue;
		case UART_LSI: line_event(port); continue;
		case UART_MSI: modem_event(port); continue;
		default: _panic("illegal uart condition code");
	}
}

void _serial_init() {
	// Traditional IO port addresses for the standard PC UARTs, COM1-COM4.
	static const uint16_t com_addrs[4] = {0x03F8, 0x02F8, 0x03E8, 0x02F8};
	static const uint8_t com_irqs[4] = {4, 3, 4, 3};
	for (unsigned i = 0; i < 4; ++i) {
		// Verify that we can communicate with a UART at this address.
		if (0 != _uart_probe(com_addrs[i])) continue;
		struct serial *port = &com[i];
		port->addr = com_addrs[i];
		channel_init(&port->tx);
		channel_init(&port->rx);
		port->signal.isr = service;
		_irq_attach(com_irqs[i], &port->signal);
		_uart_open(port->addr);
	}
}

unsigned _serial_transmit(stream_socket s, struct stream_transfer *t) {
	channel_push(&com[s].tx, t);
	_uart_tx_start(com[s].addr);
	return 0;
}

unsigned _serial_receive(stream_socket s, struct stream_transfer *t) {
	channel_push(&com[s].rx, t);
	_uart_rx_start(com[s].addr);
	return 0;
}

