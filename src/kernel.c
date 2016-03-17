// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "cpu.h"
#include "log.h"
#include "irq.h"
#include "multiboot.h"
#include "i386.h"
#include "entry.h"
#include "panic.h"
#include "uart.h"
#include "memory.h"

extern int main(int argc, char *argv[]);

void _isr_cpu(unsigned code, struct _cpu_state *regs)
{
	// Processor signalled that something fatal happened
	_panic("Processor exception %hhd at %d\n", code, regs->eip);
}

// These are defined in sys/errno.h as const, but we'll initialize them here.
int _stdin_id;
int _stdout_id;
int _stderr_id;

// Libstartc runtime calls this entrypoint function.
void _startc()
{
	assert(_multiboot);
	_memory_init(_multiboot);
	_stdin_id = _uart_open(&COM1);
	_stdout_id = _uart_open(&COM2);
	_stderr_id = _log_open();
	// Configure the IRQ table and enable interrupts.
	_irq_init();
	_sti();
	// Get the command line, if the bootloader gave us one.
	char *cmdline = "";
	if (_multiboot->flags & 1<<2) {
		cmdline = (char*)_multiboot->cmdline;
	}
	// Do something useful, someday.
	_log(INFO, "hello, world!\n");

	char *argv[1] = {cmdline};
	main(1, argv);

	// Never return from _startc.
	while (1) _hlt();
}

