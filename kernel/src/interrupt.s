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

.macro setgate vector, handler
	mov $\handler, %eax
	movw %ax, idt+(\vector*8)+0
	ror $16, %eax
	movw %ax, idt+(\vector*8)+6
.endm

.section .data
.align 8
idt: .local idt
	.rept 48
	.hword 0, 8, 0x8E00, 0
	.endr
idtr: .local idtr
	.hword ((48 * 8) - 1)	# size of table - 1: why? who knows
	.long idt
irq_table: .local irq_table
	.fill 16, 4, 0

.section .text
_interrupt_init: .global _interrupt_init
	# Install the IDT, which will dispatch exception and IRQ handlers.
	lidtl (idtr)
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
	movb $0x01, %al
	outb %al, $PIC1_DATA
	outb %al, $PIC2_DATA
	# All IRQs should be disabled at first.
	movb $0xFF, %al
	outb %al, $PIC1_DATA
	outb %al, $PIC2_DATA
	# Populate the IDT's gates with pointers to our ISR entrypoints.
	.irp index, 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
	setgate 0x0\index, _isr_exception_0\index
	setgate 0x1\index, _isr_exception_1\index
	setgate 0x2\index, .L_entry_irq_\index
	.endr
	ret

_interrupt_enable: .global _interrupt_enable
	sti
	ret

_interrupt_disable: .global _interrupt_disable
	cli
	ret

_irq_attach: .global _irq_attach
	movl 4(%esp), %ecx # irq number
	movl 8(%esp), %eax # irq_action pointer
	rol $4, %ecx
	addl $irq_table, %ecx
	xorl %edx, %edx
.L_swap_loop:
	# steal the current list head and replace it with zero
	xchg %edx, (%ecx)
	# install the list head as the new item's next pointer
	movl %edx, 4(%eax)
	# install the new item as the list head, with all the old items after it
	xchg %eax, (%ecx)
	# we expect that %eax is now zero, but if someone interrupted us to add
	# a new item onto what was a temporarily empty list, that'll be it.
	# In that case we'll swap the list head with its next pointer, then
	# repeat this process until we finally reach the end.
	testl %eax, %eax
	jz 0f
	movl 4(%eax), %edx
	jmp .L_swap_loop
0:	ret

_isr_exception00: push $0; pushal; push %esp; push $0x00; jmp 0f
_isr_exception01: push $0; pushal; push %esp; push $0x01; jmp 0f
_isr_exception02: push $0; pushal; push %esp; push $0x02; jmp 0f
_isr_exception03: push $0; pushal; push %esp; push $0x03; jmp 0f
_isr_exception04: push $0; pushal; push %esp; push $0x04; jmp 0f
_isr_exception05: push $0; pushal; push %esp; push $0x05; jmp 0f
_isr_exception06: push $0; pushal; push %esp; push $0x06; jmp 0f
_isr_exception07: push $0; pushal; push %esp; push $0x07; jmp 0f
_isr_exception08:          pushal; push %esp; push $0x08; jmp 0f
_isr_exception09: push $0; pushal; push %esp; push $0x09; jmp 0f
_isr_exception0A:          pushal; push %esp; push $0x0A; jmp 0f
_isr_exception0B:          pushal; push %esp; push $0x0B; jmp 0f
_isr_exception0C:          pushal; push %esp; push $0x0C; jmp 0f
_isr_exception0D:          pushal; push %esp; push $0x0D; jmp 0f
_isr_exception0E:          pushal; push %esp; push $0x0E; jmp 0f
_isr_exception0F: push $0; pushal; push %esp; push $0x0F; jmp 0f
_isr_exception10: push $0; pushal; push %esp; push $0x10; jmp 0f
_isr_exception11:          pushal; push %esp; push $0x11; jmp 0f
_isr_exception12: push $0; pushal; push %esp; push $0x12; jmp 0f
_isr_exception13: push $0; pushal; push %esp; push $0x13;
0:  call _interrupt_exception
	add $8, %esp
	popal
	add $4, %esp
	iret

.irp vector, 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
.L_entry_irq_\vector:
	pushal
	movl (irq_table + 0x\vector*4), %eax
0:	test %eax, %eax
	jz 1f
	pushl %eax
	call *(%eax)
	popl %eax
	movl 4(%eax), %eax
	jmp 0b
1:	movb $PIC_EOI, %al
	.if 0x\vector > 7
		outb %al, $PIC2_CMD
	.endif
	outb %al, $PIC1_CMD
	popal
	iret
.endr

