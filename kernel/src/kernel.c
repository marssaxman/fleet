// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "interrupt.h"
#include "memory.h"
#include "uart.h"
#include "debug.h"
#include <fleet/system.h>

static struct ring_list workqueue;

void post(struct task *t) {
	ring_push(&workqueue, &t->link);
}

void yield() {
	for(;;) {
		struct ring_item *i = ring_pull(&workqueue);
		if (!i) break;
		struct task *t = ring_item_struct(i, struct task, link);
		t->method(t);
	}
}

void _interrupt_exception(unsigned code, struct cpu_state *state) {
	_kprintf("Exception #%x (error %x):\n", code, state->error);
	_kprintf("EAX:%x    EBX:%x    ECX:%x    EDX:%x\n",
			state->eax, state->ebx, state->ecx, state->edx);
	_kprintf("ESI:%x    EDI:%x    ESP:%x    EBP:%x\n",
			state->esi, state->edi, state->esp, state->ebp);
	_kprintf("EFLAGS:%x              CS:%x    EIP:%x\n",
			state->eflags, state->cs, state->eip);
}

void _interrupt_irq(unsigned irq) {
	_kprintf("Unexpected IRQ #%x\n", irq);
}

extern void mic_check();

void _kernel(struct multiboot_info *multiboot) {
	ring_init(&workqueue);
	_interrupt_init();
	_memory_init(multiboot);
	_uart_init();
	mic_check();
	for (;;) {
		_interrupt_enable();
		yield();
		__asm__("hlt");
	}
}


