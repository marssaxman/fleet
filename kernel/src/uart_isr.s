# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _uart_real_init

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
.set PORT, 0 # 16 bits, base address
.set PRESENT, 2 # 8 bits, 0 = no, 1 = yes, 2 = has fifo
.set TXHEAD, 4 # ptr
.set TXTAIL, 8 # ptr
.set RXHEAD, 12 # ptr
.set RXTAIL, 16 # ptr
.set UART_STATE_SIZE, 20

.global _uart_state
.section .data
_uart_state:
.hword 0x03F8, 0
.long 0, 0, 0, 0
.hword 0x02F8, 0
.long 0, 0, 0, 0
.hword 0x03E8, 0
.long 0, 0, 0, 0
.hword 0x03F8, 0
.long 0, 0, 0, 0

.section .text
_uart_real_init:
# Install ISRs on IRQ3 and IRQ4 which serve the UARTs. We won't necessarily
# need both but it doesn't hurt to install them. The IDT structure is weird
# because it was grafted onto ancient 16-bit stuff, so the ISR address must
# be written in separate noncontiguous halves.
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
	movb %cl, _uart_has_com1
	movl $COM2, %ebx
	call config_port
	movb %cl, _uart_has_com2
	movl $COM3, %ebx
	call config_port
	movb %cl, _uart_has_com3
	movl $COM4, %ebx
	call config_port
	movb %cl, _uart_has_com4
	popl %ebx
# Clear the PIC's interrupt-inhibit flags for IRQ 3 and IRQ 4. Interrupts
# won't actually start coming in until the kernel enables them with sti.
	inb $PIC1_DATA, %al
	andb 0xE7, %al
	outb %al, $PIC1_DATA
	ret

.local config_port
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

.global _isr_com1, _isr_com2, _isr_com3, _isr_com4
.weak _isr_com1, _isr_com2, _isr_com3, _isr_com4
.local isr_irq3, isr_irq4, isr_eoi

# To avoid losing interrupts, and to avoid building a complex state machine
# covering all the possible timing variations in different UART models, we'll
# handle IRQs by disabling the interrupt and immediately signalling EOI before
# checking UART state. We'll clear the PIC disable flag before returning.
# In the meantime, we'll ignore the IIR and poll UART state until there are no
# more conditions to handle, so we don't have to worry about missing THRE
# because we got a simultaneous RBR or whatever.
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
	cmpb $0, _uart_has_com2
	je 0f
	call _isr_com2
0:	cmpb $0, _uart_has_com4
	je 1f
	call _isr_com4
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
# Check whichever ports are present and poll them until they have no further
# conditions needing attentino.
	cmpb $0, _uart_has_com1
	je 0f
	call _isr_com1
0:	cmpb $0, _uart_has_com3
	je 1f
	call _isr_com3
# Disable interrupts so we can turn IRQ4 back on, then return to whatever it
# was that was happening before this interrupt began.
1:	cli
	inb $PIC1_DATA, %al
	andb 0xEF, %al
	outb %al, $PIC1_DATA
	popal
	iret

_isr_com1:
	ret
_isr_com2:
	ret
_isr_com3:
	ret
_isr_com4:
	ret

