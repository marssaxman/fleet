// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "interrupt.h"
#include "debug.h"
#include "cpu.h"

#define PIC1_CMD 0x0020
#define PIC1_DATA 0x0021
#define PIC2_CMD 0x00a0
#define PIC2_DATA 0x00a1
#define PIC_EOI 0x0020

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10
#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#define IRQ_OFFSET 0x20
#define IRQ_COUNT 0x10

struct irq_action *handlers[IRQ_COUNT];

void _interrupt_init() {
	// Initialize the two PIC chips so external devices can raise interrupts.
	outb(PIC1_CMD, ICW1_INIT|ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT|ICW1_ICW4);
	// By default the IRQs use gates 0x00-0x0F, which overlap with the gates
	// used for CPU exceptions. This is inconvenient. Reconfigure the PICs to
	// raise their IRQ signals on gates 0x20-0x2F instead.
	outb(PIC1_DATA, IRQ_OFFSET);
	outb(PIC2_DATA, IRQ_OFFSET+8);
	// Set up the cascade configuration, using PIC1 as the master and PIC2 as
	// the slave, with IRQ2 as the cascade signal.
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	// Use 8086 mode, because that's what the ancient ritual demands.
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
	// All IRQs should be disabled at first with the exception of IRQ2, which
	// is the cascade signal from PIC2 to PIC1.
	outb(PIC1_DATA, 0xFB);
	outb(PIC2_DATA, 0xFF);
}

void _irq_attach(unsigned irq, struct irq_action *action) {
	_kassert(irq < IRQ_COUNT);
	action->next = handlers[irq];
	handlers[irq] = action;
	// Clear the PIC mask for this IRQ.
	if (irq < 8) {
		outb(PIC1_DATA, inb(PIC1_DATA) & ~(1<<irq));
	} else {
		outb(PIC2_DATA, inb(PIC2_DATA) & ~(1<<(irq-8)));
	}
}

void _interrupt(int vector) {
	if (vector >= IRQ_OFFSET && vector < (IRQ_OFFSET+IRQ_COUNT)) {
		int irq = vector - IRQ_OFFSET;
		struct irq_action *item = handlers[irq];
		while (item) {
			struct irq_action *next = item->next;
			item->isr(item);
			item = next;
		}
		// Send the EOI signal, to PIC2 if necessary and then PIC1, so they
		// can reset and continue sending future IRQs.
		if (irq >= 8) {
			outb (PIC2_CMD, PIC_EOI);
		}
		outb(PIC1_CMD, PIC_EOI);
	} else {
		_panic("unexpected interrupt #%d\n", vector);
	}
}

