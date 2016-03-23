# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# entrypoints we export
.global _uart_init

# external entrypoints we invoke
.global _uart_modem_status, _uart_line_status, _uart_tx_clear, _uart_rx_ready

# external entrypoints we override with non-weak implementations
.global _isr_IRQ3, _isr_IRQ4

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

# Field offsets for the port state struct
.set ADDR, 0x0 # short
.set INDEX, 0x2 # byte
.set FLAGS, 0x3 # byte
.set NOTIFY, 0x4 # ptr
.set TXHEAD, 0x8 # ptr
.set TXTAIL, 0xC # ptr
.set RXHEAD, 0x10 # ptr
.set RXTAIL, 0x14 # ptr
.set PORT_STATE_SIZE, 0x18

# Bit flag values
.set PORT_PRESENT, 0x01
.set PORT_HAS_FIFO, 0x2
.SET PORT_TX_START, 0x4

.section .data
port_state:
com1_state:
	.hword COM1, 0
	.long 0, 0, 0, 0, 0
com2_state:
	.hword COM2, 1
	.long 0, 0, 0, 0, 0
com3_state:
	.hword COM3, 2
	.long 0, 0, 0, 0, 0
com4_state:
	.hword COM4, 3
	.long 0, 0, 0, 0, 0

.section .text
.local config_port, isr_irq3, isr_irq4, service_port

_uart_init:
# Check each possible COM port in turn and configure it if present. We use
# %ebx as the port address parameter for internal routines because we have
# to save and restore %ebx inside an interrupt handler anyway, and this way
# it's easy to load specific register addresses without hitting the stack.
	mov $com1_state, %eax
	call config_port
	mov $com2_state, %eax
	call config_port
	mov $com3_state, %eax
	call config_port
	mov $com4_state, %eax
	call config_port
# Clear the PIC's interrupt-inhibit flags for IRQ 3 and IRQ 4. Interrupts
# won't actually start coming in until the kernel enables them with sti.
	inb $PIC1_DATA, %al
	andb 0xE7, %al
	outb %al, $PIC1_DATA
	ret

_uart_transmit:
# parameters: port number, source buffer, byte count
	mov 4(%esp), %eax
	imul $PORT_STATE_SIZE, %ax
	add $port_state, %eax
	movl 8(%esp), %ecx
	movl %ecx, TXTAIL(%eax)
	movl %ecx, TXHEAD(%eax)
	movl 12(%esp), %ecx
	addl %ecx, TXTAIL(%eax)
	ret

config_port:
# Port struct is in EAX
	pushl %ebp
	movl %eax, %ebp
	pushl %ebx
	xorl %ebx, %ebx
	movw ADDR(%eax), %bx
# Try to put the device in loopback mode, then check its modem state.
	movb $0x10, %al
	lea MCR(%ebx), %edx
	outb %al, %dx
	lea MSR(%ebx), %edx
	inb %dx, %al
# None of the status bits should be set until we try to set them.
	andb $0xF0, %al
	jnz 0f
# Still using loopback mode, try to change the modem status.
	movb $0x1F, %al
	lea MCR(%ebx), %edx
	outb %al, %dx
	lea MSR(%ebx), %edx
	inb %dx, %al
# All four bits should now be set the way we configured them.
	andb $0xF0, %al
	cmpb $0xF0, %al
	jne 0f
# We appear to have a working UART.
	orb $PORT_PRESENT, FLAGS(%ebp)
# Set its configuration the way we want.
# Use DLAB mode to configure port speed; we'll use 115.2K = fastest = best.
	lea LCR(%ebx), %edx
	mov $0x80, %al
	outb %al, %dx
	lea DLL(%ebx), %edx
	mov $1, %al
	outb %al, %dx
	lea DLH(%ebx), %edx
	xor %al, %al
	outb %al, %dx
# Clear DLAB, since its job is done, and configure 8N1 mode, again via LCR.
	lea LCR(%ebx), %edx
	mov $0x03, %al
	outb %al, %dx
# Leave DTR and RTS clear until we get buffers to play with, but set OUT2
# so the UART will ring up the PIC when it wants us to get an interrupt,
# then set the IER indicating that we want all four possible notifications.
	lea MCR(%ebx), %edx
	mov $0x80, %al
	outb %al, %dx
	lea IER(%ebx), %edx
	mov $0x0F, %al
	outb %al, %dx
# Does this UART have a working FIFO? We find out by attempting to configure it
# and seeing what the IIR state looks like afterward.
	lea FCR(%ebx), %edx
	mov $0xE7, %al
	outb %al, %dx
	lea IIR(%ebx), %edx
	inb %dx, %al
	andb $0x40, %al
	jz 1f
	orb $PORT_HAS_FIFO, FLAGS(%ebp)
# The FIFO is either broken or absent, so we'll avoid trouble by disabling it.
1:	lea FCR(%ebx), %edx
	xorl %eax, %eax
	outb %al, %dx
0:	popl %ebx
	popl %ebp
	ret

# Handling the cause of this interrupt may take a while, so we'll keep the data
# flowing as we work. Disable recurrence of this interrupt while we clear the
# port, then enable other interrupts; we'll re-enable this one when we're done.
_isr_IRQ3:
	pushal
# Disable IRQ3 while we handle its COM ports.
	inb $PIC1_DATA, %al
	orb 0x08, %al
	outb %al, $PIC1_DATA
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	sti
# We've unblocked other interrupts but won't re-trigger IRQ3.
# Check both ports, if present, and attend to their needs.
	mov $com2_state, %ebp
	call service_port
	mov $com4_state, %ebp
	call service_port
# Disable interrupts again, clear the IRQ3 disable flag, and return.
	cli
	inb $PIC1_DATA, %al
	andb 0xF7, %al
	outb %al, $PIC1_DATA
	popal
	iret

_isr_IRQ4:
	pushal
# Disable IRQ4 while we handle its COM ports.
	inb $PIC1_DATA, %al
	orb 0x10, %al
	outb %al, $PIC1_DATA
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	sti
# Check the ports and handle any conditions which need attention.
	mov $com1_state, %ebp
	call service_port
	mov $com3_state, %ebp
	call service_port
# Disable interrupts, re-enable IRQ4, and return from this ISR.
	cli
	inb $PIC1_DATA, %al
	andb 0xEF, %al
	outb %al, $PIC1_DATA
	popal
	iret

service_port:
	testb $PORT_PRESENT, FLAGS(%ebp)
	jz 0f
check_loop:
# IIR will tell us what conditions the port has experienced which need service.
# They are prioritized so we will check repeatedly until bit 1 is set showing
# that no service is needed.
	lea IIR(%ebx), %edx
	inb %dx, %al
	and $7, %al
	je modem_status
	cmp $2, %al
	je transmit_clear
	cmp $4, %al
	je receive_ready
	cmp $6, %al
	je line_status
	testb $PORT_TX_START, FLAGS(%ebp)
	jnz check_transmit
0:	ret

modem_status:
	lea MSR(%ebx), %edx
	xorl %eax, %eax
	inb %dx, %al
	pushl %eax
	call _uart_modem_status
	popl %eax
	jmp check_loop

line_status:
# OE, PE, FE, or BI has become set.
	lea LSR(%ebx), %edx
	inb %dx, %al
	ror $1, %al
	and $0x0F, %eax
	pushl %eax
	call _uart_line_status
	popl %eax
	jmp check_loop

receive_ready:
	pushl %ebp
	call _uart_rx_ready
	popl %ebp
	jmp service_port
	mov RXHEAD(%ebp), %esi
	mov RXTAIL(%ebp), %edi
# if receive buffer still has room, read RBR into buffer
# repeat until receive buffer is out of room or LSR stops indicating RBR
# if port has FIFO and receive buffer has fewer than 16 bytes left, or if
# port does not have FIFO and receive buffer is full, drop RTS/CTS/etc and
# call our event handler; if new buffer arrives, continue running
#   ...
# RBR interrupts can mask THRE, so find out whether we can transmit now too
# by falling through to check_transmit.

check_transmit:
# if there is no transmission running, start one.
	lea LSR(%ebx), %edx
	inb %dx, %al
	testb $0x40, %al
	jz check_loop
transmit_clear:
	pushl %ebp
	call _uart_tx_clear
	pop %ebp
	jmp service_port
	mov TXHEAD(%ebp), %esi
	mov TXTAIL(%ebp), %edi
# if no CTS, abort
# if no bytes in buffer, call event handler
# if still no bytes in buffer, abort
# if port has fifo, send up to 16 bytes, as many as we have
# otherwise, send one byte
	lea THR(%ebx), %edx
	jmp check_loop


