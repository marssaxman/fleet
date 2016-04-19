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

static struct task *work_head = 0;
static struct task *work_tail = 0;

void post(struct task *t) {
	bool iflag = _interrupt_suspend();
	if (work_tail) {
		work_tail->next = t;
	} else {
		work_head = t;
	}
	work_tail = t;
	_interrupt_resume(iflag);
}

void yield() {
	bool iflag = _interrupt_suspend();
	while (work_head) {
		struct task *t = work_head;
		work_head = t->next;
		if (!work_head) work_tail = 0;
		t->next = 0;
		_interrupt_resume(iflag);
		t->method(t);
		_interrupt_disable();
	}
	_interrupt_resume(iflag);
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

void _kernel(struct multiboot_info *multiboot) {
	_interrupt_init();
	_memory_init(multiboot);
	_uart_init();
	void *buffer = "Hello, world!\r\n";
	_uart_transmit(0, buffer, 15);
	for (;;) {
		_interrupt_enable();
		yield();
		__asm__("hlt");
	}
}


