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
	movw %ax, .L_idt+(\vector*8)+0
	ror $16, %eax
	movw %ax, .L_idt+(\vector*8)+6
.endm

.section .data
.align 8
.L_idt:
	.rept 48
		.hword 0, 8, 0x8E00, 0
	.endr
.L_idtr:
	.hword ((48 * 8) - 1)	# size of table - 1: why? who knows
	.long .L_idt
.L_irq_table:
	.fill 16, 4, 0
.L_irq_mask:
	.hword 0xFFFF

.section .text
_interrupt_init: .global _interrupt_init
	# Install the IDT, which will dispatch exception and IRQ handlers.
	lidtl (.L_idtr)
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
		setgate 0x0\index, .L_entry_exception_0\index
		setgate 0x1\index, .L_entry_exception_1\index
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
	pushf
	cli
	# Install the new record at the front of the list.
	movl .L_irq_table(,%ecx,4), %edx
	movl %edx, 4(%eax)
	movl %eax, .L_irq_table(,%ecx,4)
	# Clear the interrupt inhibit flag for this IRQ.
	xorl %eax, %eax
	inc %eax
	shl %cl, %eax
	not %eax
	andw .L_irq_mask, %ax
	movw %ax, .L_irq_mask
	# Update the PIC with the current inhibit mask.
	outb %al, $PIC1_DATA
	ror $8, %eax
	outb %al, $PIC2_DATA
	# Clean up and return.
	popf
	ret

.L_entry_exception_00: push $0; pushal; push %esp; push $0x00; jmp 0f
.L_entry_exception_01: push $0; pushal; push %esp; push $0x01; jmp 0f
.L_entry_exception_02: push $0; pushal; push %esp; push $0x02; jmp 0f
.L_entry_exception_03: push $0; pushal; push %esp; push $0x03; jmp 0f
.L_entry_exception_04: push $0; pushal; push %esp; push $0x04; jmp 0f
.L_entry_exception_05: push $0; pushal; push %esp; push $0x05; jmp 0f
.L_entry_exception_06: push $0; pushal; push %esp; push $0x06; jmp 0f
.L_entry_exception_07: push $0; pushal; push %esp; push $0x07; jmp 0f
.L_entry_exception_08:          pushal; push %esp; push $0x08; jmp 0f
.L_entry_exception_09: push $0; pushal; push %esp; push $0x09; jmp 0f
.L_entry_exception_0A:          pushal; push %esp; push $0x0A; jmp 0f
.L_entry_exception_0B:          pushal; push %esp; push $0x0B; jmp 0f
.L_entry_exception_0C:          pushal; push %esp; push $0x0C; jmp 0f
.L_entry_exception_0D:          pushal; push %esp; push $0x0D; jmp 0f
.L_entry_exception_0E:          pushal; push %esp; push $0x0E; jmp 0f
.L_entry_exception_0F: push $0; pushal; push %esp; push $0x0F; jmp 0f
.L_entry_exception_10: push $0; pushal; push %esp; push $0x10; jmp 0f
.L_entry_exception_11:          pushal; push %esp; push $0x11; jmp 0f
.L_entry_exception_12: push $0; pushal; push %esp; push $0x12; jmp 0f
.L_entry_exception_13: push $0; pushal; push %esp; push $0x13; jmp 0f
.L_entry_exception_14:          pushal; push %esp; push $0x14; jmp 0f
.L_entry_exception_15:          pushal; push %esp; push $0x15; jmp 0f
.L_entry_exception_16:          pushal; push %esp; push $0x16; jmp 0f
.L_entry_exception_17:          pushal; push %esp; push $0x17; jmp 0f
.L_entry_exception_18:          pushal; push %esp; push $0x18; jmp 0f
.L_entry_exception_19:          pushal; push %esp; push $0x19; jmp 0f
.L_entry_exception_1A:          pushal; push %esp; push $0x1A; jmp 0f
.L_entry_exception_1B:          pushal; push %esp; push $0x1B; jmp 0f
.L_entry_exception_1C:          pushal; push %esp; push $0x1C; jmp 0f
.L_entry_exception_1D:          pushal; push %esp; push $0x1D; jmp 0f
.L_entry_exception_1E:          pushal; push %esp; push $0x1E; jmp 0f
.L_entry_exception_1F:          pushal; push %esp; push $0x1F; jmp 0f
0:  call _interrupt_exception
	add $8, %esp
	popal
	add $4, %esp
	iret

.irp vector, 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
.L_entry_irq_\vector:
	pushal
	movl (.L_irq_table + 0x\vector*4), %eax
	# If there is an action attached to this IRQ, go execute it.
0:	test %eax, %eax
	jnz 1f
	# Send the PIC the EOI command and return.
	movb $PIC_EOI, %al
	.if 0x\vector > 7
		outb %al, $PIC2_CMD
	.endif
	outb %al, $PIC1_CMD
	popal
	iret
	# Call the action's service function, passing in the action record.
1:	pushl %eax
	call *(%eax)
	# Continue through the list of actions.
	popl %eax
	movl 4(%eax), %eax
	jmp 0b
.endr

