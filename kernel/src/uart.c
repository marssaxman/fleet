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
extern void _uart_isr3_init();
extern void _uart_isr4_init();
extern bool _uart_detect(int port);
extern bool _uart_has_fifo(int port);

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
	_kprintf("detecting uart devices\n");
	bool com1 = _uart_detect(COM1);
	_kprintf("COM1: %x\n", com1);
	bool com2 = _uart_detect(COM2);
	_kprintf("COM2: %x\n", com2);
	bool com3 = _uart_detect(COM3);
	_kprintf("COM3: %x\n", com3);
	bool com4 = _uart_detect(COM4);
	_kprintf("COM4: %x\n", com4);
	if (com2 || com4) {
		_kprintf("installing IRQ 3 handler (COM2/COM4)\n");
		_uart_isr3_init();
	}
	if (com1 || com3) {
		_kprintf("installing IRQ 4 handler (COM1/COM3)\n");
		_uart_isr4_init();
	}
	_kprintf("done initializing uarts\n");
}

