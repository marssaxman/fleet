// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "uart.h"
#include "debug.h"
#include <stdint.h>
#include <stdbool.h>

// Functions implemented in uart_isr.s
extern void _uart_real_init();
extern bool _uart_has_com1;
extern bool _uart_has_com2;
extern bool _uart_has_com3;
extern bool _uart_has_com4;

// Port base addresses
#define COM1 0x03F8
#define COM2 0x02F8
#define COM3 0x03E8
#define COM4 0x02E8

// Register offsets
#define RBR 0 // Receive buffer: DLAB=0, input
#define THR 0 // Transmit holding: DLAB=0, output
#define DLL 0 // Divisor latch low: DLAB=1
#define IER 1 // Interrupt enable: DLAB=0
#define DLH 1 // Divisor latch high: DLAB=1
#define IIR 2 // Interrupt ID, input
#define FCR 2 // FIFO control, output
#define LCR 3 // Line control and DLAB switch
#define MCR 4 // Modem control
#define LSR 5 // Line status
#define MSR 6 // Modem status

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ volatile("inb %1, %0": "=a"(ret): "Nd"(port));
	return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1": : "a"(val), "Nd"(port));
}

void _uart_init() {
	_kprintf("detecting serial ports\n");
	_uart_real_init();
	_kprintf("done initializing serial ports\n");
}

