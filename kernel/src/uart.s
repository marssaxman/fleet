# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _uart_init

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

# Flag array

# Field offsets for the port state struct
.set TXHEAD, 0x0 # ptr
.set TXTAIL, 0x4 # ptr
.set RXHEAD, 0x8 # ptr
.set RXTAIL, 0xC # ptr
.set PORT_STATE_SIZE, 0x10

.section .data
port_present: .skip 4
port_state: .skip 4 * PORT_STATE_SIZE

.local config_port, isr_irq3, isr_irq4, service_port

.section .text
_uart_init:
# Install ISRs on IRQ3 and IRQ4 which serve the UARTs. We won't necessarily
# need both but it doesn't hurt to install them. The IDT structure is weird
# because it was grafted onto ancient 16-bit structures, so the ISR address
# must be written as a pair of noncontiguous half-words.
	movl $isr_irq3, %eax
	movw %ax, _idt_signals + (3 * 8) + 0
	shrl $16, %eax
	movw %ax, _idt_signals + (3 * 8) + 6
	movl $isr_irq4, %eax
	movw %ax, _idt_signals + (4 * 8) + 0
	shrl $16, %eax
	movw %ax, _idt_signals + (4 * 8) + 6
# Check each possible COM port in turn and configure it if present. We use
# %ebx as the port address parameter for internal routines because we have
# to save and restore %ebx inside an interrupt handler anyway, and this way
# it's easy to load specific register addresses without hitting the stack.
	pushl %ebx
	movl $COM1, %ebx
	call config_port
	movb %cl, port_present+0
	movl $COM2, %ebx
	call config_port
	movb %cl, port_present+1
	movl $COM3, %ebx
	call config_port
	movb %cl, port_present+2
	movl $COM4, %ebx
	call config_port
	movb %cl, port_present+3
	popl %ebx
# Clear the PIC's interrupt-inhibit flags for IRQ 3 and IRQ 4. Interrupts
# won't actually start coming in until the kernel enables them with sti.
	inb $PIC1_DATA, %al
	andb 0xE7, %al
	outb %al, $PIC1_DATA
	ret

config_port:
# Port address parameter is supplied in %ebx and return val is in %ecx, which
# will be 0 if we don't find a UART, 1 if a UART is present but lacks a FIFO,
# and 2 if it is a 16550A or newer with a working FIFO. We won't change %ebx
# or any nonvolatile register, but of course we'll trash %EAX and %EDX.
	xorl %ecx, %ecx
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
# We appear to have a working UART. Set its configuration the way we want.
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
# and seeing what the IIR state looks like afterward. We'll return 2 if it has
# a FIFO but fall back to 1 if the chip is older than the 16550A.
	movb $2, %cl
	lea FCR(%ebx), %edx
	mov $0xE7, %al
	outb %al, %dx
	lea IIR(%ebx), %edx
	inb %dx, %al
	andb $0x40, %al
	jnz 0f
# The FIFO is either broken or absent, so we'll avoid trouble by disabling it.
	movb $1, %cl
	lea FCR(%ebx), %edx
	xorl %eax, %eax
	outb %al, %dx
0:	ret

# To avoid losing interrupts without having to rig up a complex, fragile state
# machine covering all the possible timing behaviors of all the different UART
# models, we'll handle IRQs by disabling the interrupt, immediately signalling
# EOI and re-enabling other interrupts so we don't block, then polling the
# UARTs associated with this interrupt until they report no more conditions we
# need to handle. Then we'll re-enable the IRQ and return. This way we don't
# have to worry about missing THRE because of a simultaneous RBR condition or
# anything like that.
isr_irq3:
	pushal
	inb $PIC1_DATA, %al
	orb 0x08, %al
	outb %al, $PIC1_DATA
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	sti
# We've unblocked other interrupts but won't re-trigger IRQ3.
# Check both ports, if present, and attend to their needs.
	cmpb $0, port_present+1
	je 0f
	mov $COM2, %ebx
	mov $port_state + 1*PORT_STATE_SIZE, %ebp
	call service_port
0:	cmpb $0, port_present+3
	je 1f
	mov $COM4, %ebx
	mov $PORT_STATE + 3*PORT_STATE_SIZE, %ebp
	call service_port
# Disable interrupts again, then clear the PIC disable flag for IRQ3. We'll
# implicitly re-enable interrupts through iret.
1:	cli
	inb $PIC1_DATA, %al
	andb 0xF7, %al
	outb %al, $PIC1_DATA
	popal
	iret

isr_irq4:
	pushal
# Disable IRQ4 while we handle its COM ports.
	inb $PIC1_DATA, %al
	orb 0x10, %al
	outb %al, $PIC1_DATA
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	sti
# Check the ports and handle any conditions which need attention.
	cmpb $0, port_present+0
	je 0f
	mov $COM1, %ebx
	mov $port_state + 0*PORT_STATE_SIZE, %ebp
	call service_port
0:	cmpb $0, port_present+2
	je 1f
	mov $COM3, %ebx
	mov $port_state + 2*PORT_STATE_SIZE, %ebp
	call service_port
# Disable interrupts so we can turn IRQ4 back on, then return to whatever it
# was that was happening before this interrupt began.
1:	cli
	inb $PIC1_DATA, %al
	andb 0xEF, %al
	outb %al, $PIC1_DATA
	popal
	iret

.global _uart_modem_status, _uart_error, _uart_rx, _uart_tx
.weak _uart_modem_status, _uart_error, _uart_rx, _uart_tx

service_port:
# EBX contains the port address, EBP the base of the port state struct.
# The IIR contains the reason the interrupt was raised, if any. If the UART
# did raise a signal, bit 0 will be 0; if it did not signal, it will be 1.
	lea IIR(%ebx), %edx
	inb %dx, %al
	test $1, %al
	jnz 0f
# If this was a modem status change, read the MSR; it's a flow control change
# or a carrier detect/loss.
	andb $6, %al
	jnz 1f
	lea MSR(%ebx), %edx
	xorl %eax, %eax
	inb %dx, %al
	pushl %eax
	pushl %ebx
	call _uart_modem_status
	add $8, %esp
	jmp 0f
# For all other interrupts, we'll read LSR and handle all conditions indicated
# regardless of the reason for the interrupt; not only that, we'll loop back
# and continue checking LSR until we've done all the work we can do.
1:	lea LSR(%ebx), %edx
	xorl %eax, %eax
	inb %dx, %al
# check for transmission errors
	test $0x9E, %al
	jz 2f
	pushl %eax
	pushl %ebx
	call _uart_error
	add $8, %esp
	jmp 4f
# check for incoming data
2:	test $1, %al
	jz 3f
	lea RBR(%ebx), %edx
	xorl %eax, %eax
	inb %dx, %al
	pushl %eax
	pushl %ebx
	call _uart_rx
	add $8, %esp
# check for clear transmit line
3:	test $0x40, %al
	jz 4f
4:	ret
0:	ret

_uart_modem_status:
	ret

_uart_error:
	ret

_uart_rx:
	ret





service:
# does this port need attention? If so, what sort? IIR has the answer: bit zero
# is high when the port is happy and low when a condition needs to be resolved.
# Bits 2 and 3 indicate the four categories of condition needing service. The
# device will return them in priority order until we've resolved them all.
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
	ret

modem_status:
	# 
	lea MSR(%ebx), %edx
	inb %dx, %al
	# ...
	jmp service

transmit_clear:
	mov TXHEAD(%ebp), %esi
	mov TXTAIL(%ebp), %edi
# if no CTS, abort
# if 
# if no bytes in buffer, call event handler
# if still no bytes in buffer, abort
# if port has fifo, send up to 16 bytes, as many as we have
# otherwise, send one byte
	lea THRE(%ebx), %edx
	# ...
	jmp service

receive_ready:
	mov RXHEAD(%ebp), %esi
	mov RXTAIL(%ebp), %edi
# if receive buffer still has room, read RBR into buffer
# repeat until receive buffer is out of room or LSR stops indicating RBR
# if port has FIFO and receive buffer has fewer than 16 bytes left, or if
# port does not have FIFO and receive buffer is full, drop RTS/CTS/etc and
# call our event handler; if new buffer arrives, continue running
	jmp service

line_status:
# OE, PE, FE, or BI has become set.
	lea LSR(%ebx), %edx
	inb %dx, %al
	# ...
	jmp service

