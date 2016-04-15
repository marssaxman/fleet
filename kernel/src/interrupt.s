# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# Exports
.global _interrupt_init, _interrupt_enable, _interrupt_disable
.global _interrupt_suspend, _interrupt_resume

# Override this function to handle exceptions
.weak _interrupt_exception
.global _interrupt_exception

# Override this function to be notified when unhandled IRQs occur
.weak _interrupt_irq
.global _interrupt_irq

# Or override an individual IRQ entrypoint to handle that particular signal
.weak _isr_IRQ0, _isr_IRQ1, _isr_IRQ2, _isr_IRQ3, _isr_IRQ4, _isr_IRQ5
.weak _isr_IRQ6, _isr_IRQ7, _isr_IRQ8, _isr_IRQ9, _isr_IRQ10, _isr_IRQ11
.weak _isr_IRQ12, _isr_IRQ13, _isr_IRQ14, _isr_IRQ15
.global _isr_IRQ0, _isr_IRQ1, _isr_IRQ2, _isr_IRQ3, _isr_IRQ4, _isr_IRQ5
.global _isr_IRQ6, _isr_IRQ7, _isr_IRQ8, _isr_IRQ9, _isr_IRQ10, _isr_IRQ11
.global _isr_IRQ12, _isr_IRQ13, _isr_IRQ14, _isr_IRQ15

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC2_CMD, 0x00a0
.set PIC2_DATA, 0x00a1
.set PIC_EOI, 0x0020

.section .text
_interrupt_init:
# Interrupts should already be disabled, but be sure before tinkering.
	cli
# Register our IDT structure with the CPU.
	lidt idtr
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
# Populate the IDT with the default interrupt service entrypoints. We can't
# initialize the table statically because there is no way to specify just half
# of an entrypoint's address when generating data blocks.
	mov $_isr_exception00, %eax
	movw %ax, exceptions+(0x00*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x00*8)+6
	mov $_isr_exception01, %eax
	movw %ax, exceptions+(0x01*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x01*8)+6
	mov $_isr_exception02, %eax
	movw %ax, exceptions+(0x02*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x02*8)+6
	mov $_isr_exception03, %eax
	movw %ax, exceptions+(0x03*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x03*8)+6
	mov $_isr_exception04, %eax
	movw %ax, exceptions+(0x04*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x04*8)+6
	mov $_isr_exception05, %eax
	movw %ax, exceptions+(0x05*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x05*8)+6
	mov $_isr_exception06, %eax
	movw %ax, exceptions+(0x06*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x06*8)+6
	mov $_isr_exception07, %eax
	movw %ax, exceptions+(0x07*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x07*8)+6
	mov $_isr_exception08, %eax
	movw %ax, exceptions+(0x08*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x08*8)+6
	mov $_isr_exception09, %eax
	movw %ax, exceptions+(0x09*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x09*8)+6
	mov $_isr_exception0A, %eax
	movw %ax, exceptions+(0x0A*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0A*8)+6
	mov $_isr_exception0B, %eax
	movw %ax, exceptions+(0x0B*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0B*8)+6
	mov $_isr_exception0C, %eax
	movw %ax, exceptions+(0x0C*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0C*8)+6
	mov $_isr_exception0D, %eax
	movw %ax, exceptions+(0x0D*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0D*8)+6
	mov $_isr_exception0E, %eax
	movw %ax, exceptions+(0x0E*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0E*8)+6
	mov $_isr_exception0F, %eax
	movw %ax, exceptions+(0x0F*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x0F*8)+6
	mov $_isr_exception10, %eax
	movw %ax, exceptions+(0x10*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x10*8)+6
	mov $_isr_exception11, %eax
	movw %ax, exceptions+(0x11*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x11*8)+6
	mov $_isr_exception12, %eax
	movw %ax, exceptions+(0x12*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x12*8)+6
	mov $_isr_exception13, %eax
	movw %ax, exceptions+(0x13*8)+0
	ror $16, %eax
	movw %ax, exceptions+(0x13*8)+6
	mov $_isr_IRQ0, %eax
	movw %ax, signals+(0x00*8)+0
	ror $16, %eax
	movw %ax, signals+(0x00*8)+6
	mov $_isr_IRQ1, %eax
	movw %ax, signals+(0x01*8)+0
	ror $16, %eax
	movw %ax, signals+(0x01*8)+6
	mov $_isr_IRQ2, %eax
	movw %ax, signals+(0x02*8)+0
	ror $16, %eax
	movw %ax, signals+(0x02*8)+6
	mov $_isr_IRQ3, %eax
	movw %ax, signals+(0x03*8)+0
	ror $16, %eax
	movw %ax, signals+(0x03*8)+6
	mov $_isr_IRQ4, %eax
	movw %ax, signals+(0x04*8)+0
	ror $16, %eax
	movw %ax, signals+(0x04*8)+6
	mov $_isr_IRQ5, %eax
	movw %ax, signals+(0x05*8)+0
	ror $16, %eax
	movw %ax, signals+(0x05*8)+6
	mov $_isr_IRQ6, %eax
	movw %ax, signals+(0x06*8)+0
	ror $16, %eax
	movw %ax, signals+(0x06*8)+6
	mov $_isr_IRQ7, %eax
	movw %ax, signals+(0x07*8)+0
	ror $16, %eax
	movw %ax, signals+(0x07*8)+6
	mov $_isr_IRQ8, %eax
	movw %ax, signals+(0x08*8)+0
	ror $16, %eax
	movw %ax, signals+(0x08*8)+6
	mov $_isr_IRQ9, %eax
	movw %ax, signals+(0x09*8)+0
	ror $16, %eax
	movw %ax, signals+(0x09*8)+6
	mov $_isr_IRQ10, %eax
	movw %ax, signals+(0x0A*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0A*8)+6
	mov $_isr_IRQ11, %eax
	movw %ax, signals+(0x0B*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0B*8)+6
	mov $_isr_IRQ12, %eax
	movw %ax, signals+(0x0C*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0C*8)+6
	mov $_isr_IRQ13, %eax
	movw %ax, signals+(0x0D*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0D*8)+6
	mov $_isr_IRQ14, %eax
	movw %ax, signals+(0x0E*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0E*8)+6
	mov $_isr_IRQ15, %eax
	movw %ax, signals+(0x0F*8)+0
	ror $16, %eax
	movw %ax, signals+(0x0F*8)+6
	ret

# Default exception and signal handlers
_interrupt_exception: hlt; ret
_interrupt_signal: ret

# Enable or disable interrupts
_interrupt_enable: sti; ret
_interrupt_disable: cli; ret

# If interrupts are enabled, disable them, then return previous state
_interrupt_suspend:
	pushf
	pop %eax
	testb $0x20, %al
	setnz %al
	jnz 0f
	cli
0:	ret

# If previous state was true, enable interrupts again
_interrupt_resume:
	movb 4(%esp),%al
	jnz 0f
	sti
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

_isr_IRQ0: pushal; push $0; jmp 0f
_isr_IRQ1: pushal; push $1; jmp 0f
_isr_IRQ2: pushal; push $2; jmp 0f
_isr_IRQ3: pushal; push $3; jmp 0f
_isr_IRQ4: pushal; push $4; jmp 0f
_isr_IRQ5: pushal; push $5; jmp 0f
_isr_IRQ6: pushal; push $6; jmp 0f
_isr_IRQ7: pushal; push $7;
0:	call _interrupt_signal
	add $4, %esp
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	popal
	iret

_isr_IRQ8: pushal; push $8; jmp 0f
_isr_IRQ9: pushal; push $9; jmp 0f
_isr_IRQ10: pushal; push $10; jmp 0f;
_isr_IRQ11: pushal; push $11; jmp 0f;
_isr_IRQ12: pushal; push $12; jmp 0f;
_isr_IRQ13: pushal; push $13; jmp 0f;
_isr_IRQ14: pushal; push $14; jmp 0f;
_isr_IRQ15: pushal; push $15;
0:	call _interrupt_signal
	add $4, %esp
	movb $PIC_EOI, %al
	outb %al, $PIC2_CMD
	outb %al, $PIC1_CMD
	popal
	iret

.section .data; .align 8; .local idt, idtr
idt:
exceptions: .rept 20; .hword 0, 8, 0x8E00, 0; .endr
reserved:   .rept 12; .hword 0, 0, 0x0000, 0; .endr
signals:    .rept 16; .hword 0, 8, 0x8E00, 0; .endr
idtr:
	.hword ((48 * 8) - 1)	# size of table - 1: why? who knows
	.long idt

