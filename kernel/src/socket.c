// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "socket.h"
#include "debug.h"

struct socket {
	int handle;
	struct ring_item pool;
	struct socket_methods *op;
	void *data;
} table[OPEN_MAX];

static struct ring_list live_sockets;
static struct ring_list free_sockets;

void _socket_init() {
	ring_init(&live_sockets);
	ring_init(&free_sockets);
	for (unsigned i = 0; i < OPEN_MAX; ++i) {
		table[i].handle = i;
		ring_push(&free_sockets, &table[i].pool);
	}
}

int _socket_alloc(struct socket_methods *op, void *data) {
	struct ring_item *i = ring_pull(&free_sockets);
	struct socket *s = container_of(i, struct socket, pool);
	ring_push(&live_sockets, i);
	s->op = op;
	s->data = data;
	if (op->open) op->open(s);
	return s->handle;
}

void _socket_free(int handle) {
	_kassert(handle >= 0 && handle < OPEN_MAX);
	struct socket *s = &table[handle];
	_kassert(s->op);
	s->op = 0;
	s->data = 0;
	ring_remove(&s->pool);
	ring_push(&free_sockets, &s->pool);
}

