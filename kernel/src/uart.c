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

static bool setup_one(uint16_t port) {
	if (!_uart_detect(port)) return false;
	_kprintf("serial port detected at %x\n", port);
	// Switch DLAB on so we can set port speed.
	outb(port + LCR, 0x80);
	// Use divisor 1 = 115200 bps = fastest = best.
	outb(port + DLL, 1);
	outb(port + DLH, 0);
	// Switch DLAB back off and configure 8N1 mode, which we can do at once
	// since DLAB is switched by the high bit in the LCR.
	outb(port + LCR, 0x03);
	// Clear the MCR; we're not ready to send or to receive.
	outb(port + MCR, 0);
	// We aren't listening for interrupts yet, either; we'll enable those when
	// we have some actual work to do. 
	outb(port + IER, 0);
	return true;
}

void _uart_init() {
	_kprintf("detecting serial ports\n");
	bool com1 = setup_one(COM1);
	bool com2 = setup_one(COM2);
	bool com3 = setup_one(COM3);
	bool com4 = setup_one(COM4);
	if (com2 || com4) {
		_kprintf("installing IRQ 3 handler (COM2/COM4)\n");
		_uart_isr3_init();
	}
	if (com1 || com3) {
		_kprintf("installing IRQ 4 handler (COM1/COM3)\n");
		_uart_isr4_init();
	}
	_kprintf("done initializing serial ports\n");
}
