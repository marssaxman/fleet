// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "interrupt.h"
#include "uart.h"
#include "debug.h"
#include "kernel.h"

static struct process *process_queue, **process_link;

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

void _kernel() {
	process_link = &process_queue;
	_interrupt_init();
	_uart_init();
	const char *message = "Hello, world!\r\n";
	_uart_transmit(0, message, 15);
	for (;;) {
		_poll();
		_interrupt_enable();
		__asm__("hlt");
	}
}

void _poll() {
	bool iflag = _interrupt_suspend();

	_interrupt_resume(iflag);
}

void _start(struct process *p) {
	kassert(p->process_queue == 0);
	bool iflag = _interrupt_suspend();

	_interrupt_resume(iflag);
}

void _stop(struct process *p) {
	kassert(p->process_queue != 0);
	bool iflag = _interrupt_suspend();

	_interrupt_resume(iflag);
}

void _schedule(struct process *p, struct task *t) {
	kassert(t->task_queue == 0);
	bool iflag = _interrupt_suspend();

	_interrupt_resume(iflag);
}

void _post(struct event *e) {
	bool iflag = _interrupt_suspend();

	_interrupt_resume(iflag);
}


