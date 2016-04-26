# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC2_CMD, 0x00a0
.set PIC2_DATA, 0x00a1
.set PIC_EOI, 0x0020

.set ICW1_ICW4, 0x01
.set ICW1_SINGLE, 0x02
.set ICW1_INTERVAL4, 0x04
.set ICW1_LEVEL, 0x08
.set ICW1_INIT, 0x10
.set ICW4_8086, 0x01
.set ICW4_AUTO, 0x02
.set ICW4_BUF_SLAVE, 0x08
.set ICW4_BUF_MASTER, 0x0C
.set ICW4_SFNM, 0x10

.text
# void _pic_init();
_pic_init: .global _pic_init
	# ICW1: begin initialization sequence.
	movb $(ICW1_INIT|ICW1_ICW4), %al
	outb %al, $PIC1_CMD
	outb %al, $PIC2_CMD
	# ICW2: interrupt descriptor table offset, added to the PIC's IRQ numbers.
	# By default the IRQs use gates 0x00-0x0F, which overlap with the gates
	# used for CPU exceptions. This is inconvenient. Reconfigure the PICs to
	# raise their IRQ signals on gates 0x20-0x2F instead.
	movb $0x20, %al
	outb %al, $PIC1_DATA
	movb $0x28, %al
	outb %al, $PIC2_DATA
	# ICW3: cascade configuration, using PIC1 as the master and PIC2 as the
	# slave, with IRQ2 as the cascade signal.
	movb $0x04, %al
	outb %al, $PIC1_DATA
	movb $0x02, %al
	outb %al, $PIC2_DATA
	# ICW4: use 8086 mode.
	movb $ICW4_8086, %al
	outb %al, $PIC1_DATA
	outb %al, $PIC2_DATA
	# All IRQs should be disabled at first with the exception of IRQ2, which
	# is the cascade signal from PIC2 to PIC1.
	movb $0xFB, %al
	outb %al, $PIC1_DATA
	movb $0xFF, %al
	outb %al, $PIC2_DATA
	ret

# void _pic_eoi();
_pic_eoi: .global _pic_eoi
	movb $PIC_EOI, %al
	outb %al, $PIC2_CMD
	outb %al, $PIC1_CMD
	ret

# void _pic_mask(uint16_t);
_pic_mask: .global _pic_mask
	movw 4(%esp), %ax
	outb %al, $PIC1_DATA
	ror $8, %eax
	outb %al, $PIC2_DATA
	ret

