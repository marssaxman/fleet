// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdbool.h>
#include "multiboot.h"
#include "interrupt.h"
#include "uart.h"
#include "debug.h"

void _kernel(uint32_t magic, struct multiboot_info *multiboot) {
	_kprintf("HELLO, KERNEL WORLD\n");
	_interrupt_init();
	_kprintf("WE HAVE INTERRUPTS\n");
	_uart_init();
	_kprintf("com1 present: %s\n", _com1_present? "yes": "no");
	_kprintf("com2 present: %s\n", _com2_present? "yes": "no");
	_kprintf("com3 present: %s\n", _com3_present? "yes": "no");
	_kprintf("com4 present: %s\n", _com4_present? "yes": "no");
}

