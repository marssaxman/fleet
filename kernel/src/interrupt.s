# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _interrupt_init, _interrupt_enable, _interrupt_disable
.global _idt_exceptions, _idt_signals

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC2_CMD, 0x00a0
.set PIC2_DATA, 0x00a1
.set PIC_EOI, 0x0020

.section text
_interrupt_init:
	# Interrupts should already be disabled, but make sure of it before we
	# start tinkering; then load the pointer to our IDT.
	cli
	lidt (idtr)
	# By default the IRQs use gates 0x00-0x0F, which overlap with the gates
	# used for CPU exceptions, which is inconvenient. Reconfigure the PICs
	# to raise IRQ signals on gates 0x20-0x2F instead.
	# Start PIC initialization and enable ICW4.
	movb $0x11, %al
	outb %al, $PIC1_CMD
	outb %al, $PIC2_CMD
	# Set up the IDT vector table offsets.
	movb $0x20, %al
	outb %al, $PIC1_DATA
	movb $0x28, %al
	outb %al, $PIC2_DATA
	# Configure the master/slave wiring.
	movb $0x04, %al
	outb %al, $PIC1_DATA
	movb $0x02, %al
	outb %al, $PIC2_DATA
	# Use 8086 mode and other typical settings.
	movl $0x01, %eax
	outb %al, $PIC1_DATA
	outb %al, $PIC2_DATA
	# All IRQs should be disabled at first.
	movl $0xFF, %eax
	outb %al, $PIC1_DATA
	outb %al, $PIC2_DATA
	ret

_interrupt_enable:
	sti
	ret

_interrupt_disable:
	cli
	ret

.section .data
.align 8
.local idt, idtr
idt:
_idt_exceptions:
	.skip (20 * 8)
# reserved but unused
	.skip (12 * 8)
_idt_signals:
	.skip (16 * 8)
idtr:
	.hword ((48 * 8) - 1)	# size of table - 1: why? who knows
	.long idt

