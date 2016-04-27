// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "memory.h"
#include "serial.h"
#include "debug.h"
#include "socket.h"
#include "idt.h"
#include "pic.h"
#include "irq.h"
#include "cpu.h"

static struct ring_list eventqueue;

void post(struct event *e) {
	ring_push(&eventqueue, &e->link);
}

void yield() {
	for(;;) {
		struct ring_item *i = ring_pull(&eventqueue);
		if (!i) break;
		struct event *e = container_of(i, struct event, link);
		if (e->handler) {
			e->handler(e);
		}
	}
}

static bool done = false;

static void check_proc(struct event *e) {
	_kprintf("mic check complete\n");
	done = true;
}

void mic_check() {
	static struct stream_transfer hello_xfer;
	hello_xfer.request.buffer = "Hello, world!\r\n";
	hello_xfer.request.length = 15;
	hello_xfer.signal.handler = check_proc;
	_serial_transmit(0, &hello_xfer);
}

void _kernel(struct multiboot_info *multiboot) {
	ring_init(&eventqueue);
	_idt_init();
	_pic_init();
	_irq_init();
	_memory_init(multiboot);
	_socket_init();
	_serial_init();
	_cpu_int_enable();
	mic_check();
	while (!done) {
		yield();
	}
	_cpu_reset();
}


