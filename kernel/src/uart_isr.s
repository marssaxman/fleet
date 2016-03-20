# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _uart_isr3_init, _uart_isr4_init
.global _uart_real_init, _uart_detect, _uart_setup, _uart_has_fifo

.global _uart_has_com1, _uart_has_com2, _uart_has_com3, _uart_has_com4
.section .data
_uart_has_com1: .byte 0
_uart_has_com2: .byte 0
_uart_has_com3: .byte 0
_uart_has_com4: .byte 0

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC_EOI, 0x0020

# Port base addresses
.set COM1, 0x03F8
.set COM2, 0x02F8
.set COM3, 0x03E8
.set COM4, 0x02E8

# Register offsets
.set RBR, 0	# Receive buffer: DLAB=0, input
.set THR, 0	# Transmit holding: DLAB=0, output
.set DLL, 0	# Divisor latch low: DLAB=1
.set IER, 1	# Interrupt enable: DLAB=0
.set DLH, 1	# Divisor latch high: DLAB=1
.set IIR, 2	# Interrupt ID, input
.set FCR, 2	# FIFO control, output
.set LCR, 3	# Line control and DLAB switch
.set MCR, 4	# Modem control
.set LSR, 5	# Line status
.set MSR, 6	# Modem status

.section .text
_uart_real_init:
	pushl $COM1
	call _uart_detect
	movb %al, _uart_has_com1
	jz 1f
	call _uart_setup
1:	movl $COM2, (%esp)
	call _uart_detect
	movb %al, _uart_has_com2
	jz 2f
	call _uart_setup
2:	movl $COM3, (%esp)
	call _uart_detect
	movb %al, _uart_has_com3
	jz 3f
	call _uart_setup
3:	movl $COM4, (%esp)
	call _uart_detect
	movb %al, _uart_has_com4
	jz 4f
	call _uart_setup
4:	popl %eax
	ret

_uart_detect:
	pushl %ebx # will use this for port base address
	# stack: saved-ebx, retaddr, port
	movl 8(%esp), %ebx
	# Save the old MCR value so we can restore it after tinkering.
	lea MCR(%ebx), %edx
	inb %dx, %al
	pushl %eax
	# stack: saved-MCR, saved-ebx, retaddr, param
	# Try to put the device in loopback mode.
	movb $0x10, %al
	lea MCR(%ebx), %edx
	outb %al, %dx
	# Check the MSR: none of CTS, DTR, ring, or carrier detect should be set.
	lea MSR(%ebx), %edx
	inb %dx, %al
	andb $0xF0, %al
	setz %al
	jnz 0f
	# Still using loopback mode, try to set those four status flags.
	movb $0x1F, %al
	lea MCR(%ebx), %edx
	outb %al, %dx
	# Does the MSR show that all four flags are set as we expect?
	lea MSR(%ebx), %edx
	inb %dx, %al
	andb $0xF0, %al
	cmpb $0xF0, %al
	sete %al
	jne 0f
	# The port appears to have a working UART, so restore its old MCR value.
	movb (%esp), %al
	lea MCR(%ebx), %edx
	outb %al, %dx
	# Return 1 to indicate UART present.
	mov $1, %al
0:	add $4, %esp # lose the saved MCR
	popl %ebx # definitely need to restore this
	ret

_uart_has_fifo:
	pushl %ebx # will use this for port base address
	# stack: saved-ebx, retaddr, port
	movl 8(%esp), %ebx
	# try to put the device into loopback mode
	mov $0xE7, %al
	lea FCR(%ebx), %edx
	outb %al, %dx
	# check the IIR: are *both* high bits set?
	lea IIR(%ebx), %edx
	inb %dx, %al
	andb $0xC0, %al
	cmpb $0xC0, %al
	sete %al
	# Save the return value; we need to clear the FCR for the time being.
	push %eax
	xorl %eax, %eax
	lea FCR(%ebx), %edx
	outb %al, %dx
	# We're done: return what we discovered.
	popl %eax
	popl %ebx
	ret

_uart_setup:
	pushl %ebx
	movl 8(%esp), %ebx
	# Set DLAB so we can configure port speed.
	lea LCR(%ebx), %edx
	mov $0x80, %al
	outb %al, %dx
	# Set DLL = 1 and DLH = 0 for divisor 1, which is 115.2K = fast = good.
	lea DLL(%ebx), %edx
	mov $1, %al
	outb %al, %dx
	lea DLH(%ebx), %edx
	xor %al, %al
	outb %al, %dx
	# Clear DLAB because setting the speed is all it does.
	# We can configure 8N1 mode at the same time since it's all on LCR.
	lea LCR(%ebx), %edx
	mov $0x03, %al
	outb %al, %dx
	# Clear the MCR so it's clear we are not ready to send or receive.
	lea MCR(%ebx), %edx
	xor %al, %al
	outb %al, %dx
	# We're not ready to receive any interrupts yet, either.
	lea IER(%ebx), %edx
	outb %al, %dx
	# Some day we might want to configure the FIFO here.
	popl %ebx
	ret

_uart_isr3_init:
	# Poke the halves of the ISR address into the IDT gate.
	movl $isr_irq3, %eax
	movw %ax, _idt_signals + (3 * 8) + 0
	shrl $16, %eax
	movw %ax, _idt_signals + (3 * 8) + 6
	# Clear the PIC disable flag for IRQ 3.
	inb $PIC1_DATA, %al
	andb (1 << 3), %al
	outb %al, $PIC1_DATA
	ret

_uart_isr4_init:
	# Poke the ISR address into the gate record.
	movl $isr_irq4, %eax
	movw %ax, _idt_signals + (4 * 8) + 0
	shrl $16, %eax
	movw %ax, _idt_signals + (4 * 8) + 6
	# Clear the PIC disable flag on IRQ 4.
	inb $PIC1_DATA, %al
	andb (1 << 4), %al
	outb %al, $PIC1_DATA
	ret

.local isr_irq3, isr_irq4, isr_eoi

# COM2 and COM4 share IRQ3.
isr_irq3:
	pushal
	cld
	push $8
	push $FLOO
	call _console_write
	jmp isr_eoi

# COM1 and COM3 share IRQ4.
isr_irq4:
	pushal
	cld
	push $8
	push $DLEB
	call _console_write
	jmp isr_eoi

isr_eoi:
	# Reset the PIC; send the end of exception signal
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	popal
	iret

.section .data
FLOO: .ascii "YO MOMMA";
DLEB: .ascii "SPRIBBLE";


