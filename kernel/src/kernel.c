// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "multiboot.h"
#include "interrupt.h"
#include "uart.h"
#include "debug.h"

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

void _kernel(uint32_t magic, struct multiboot_info *multiboot) {
	_interrupt_init();
	_uart_init();
	_interrupt_enable();

	const char *src =
"In early days of telecommunication, errand-boys and optical signals (flags,\n"
"lights, clouds of smoke) were the only methods of transmitting information\n"
"across long distances. With increasing requirements on speed and growing\n"
"amount of information, more practical methods were developed. One milestone\n"
"was the first wire-bound transmission on May 24th, 1844 (\"What hath God\n"
"wrought\", using the famous Morse alphabet). Well, technology improved a \n"
"bit, and soon there were machines that could be used like typewriters, \n"
"except that you typed not only on your own sheet of paper but also on \n"
"somebody else's. The only thing that has changed on the step from the \n"
"teletype to your PC regarding serial communications is speed.\n";
	unsigned len = 0;
	for (const char *s = src; *s; ++s) { ++len; }
	_kprintf("transmitting %i chars\n", len);
	_uart_transmit(0, src, len);
	char buf[12];
	_uart_receive(0, buf, 12);
	while (1) {}
}

