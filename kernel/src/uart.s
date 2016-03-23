# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# entrypoints we export
.global _uart_init, _uart_transmit

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

# Flag definitions
.set IER_RBRI, 0x01 # Enable data buffer ready interrupt
.set IER_THRI, 0x02 # Enable transmitter clear interrupt
.set IER_LSI, 0x04 # Enable line status interrupt
.set IER_MSI, 0x08 # Enable modem status interrupt
.set IIR_NONE, 0x01 # No interrupts pending
.set IIR_ID_MASK, 0x06 # produces one of the following:
.set IIR_MSI, 0x00 # Modem status change
.set IIR_THRI, 0x02 # Transmitter holding register empty
.set IIR_RBRI, 0x04 # Receive buffer interrupt
.set IIR_LSI, 0x06 # Line status change
.set MCR_DTR, 0x01 # Control DTR signal
.set MCR_RTS, 0x02 # Control RTS signal
.set MCR_OUT1, 0x04 # Misc control signal 1
.set MCR_OUT2, 0x08 # Misc control signal 2
.set MCR_LOOP, 0x10 # Loopback mode
.set MSR_DCTS, 0x01 # CTS change
.set MSR_DDSR, 0x02 # DSR change
.set MSR_TERI, 0x04 # Ring indicator ended
.set MSR_DDCD, 0x08 # DCD change
.set MSR_CTS, 0x10 # Clear to send
.set MSR_DSR, 0x20 # Data set ready (present)
.set MSR_RI, 0x40 # Ring indicator
.set MSR_DCD, 0x80 # Data carrier detected

# Field offsets for the port state struct
.set ADDR, 0x0 # short
.set INDEX, 0x2 # byte
.set FLAGS, 0x3 # byte
.set TXHEAD, 0x4 # ptr
.set TXTAIL, 0x8 # ptr
.set RXHEAD, 0xC # ptr
.set RXTAIL, 0x10 # ptr
.set PORT_STATE_SIZE, 0x14

# Bit flag values
.set PORT_PRESENT, 0x01
.set PORT_HAS_FIFO, 0x2

.section .data
port_state:
com1_state:
	.hword COM1, 0
	.long 0, 0, 0, 0
com2_state:
	.hword COM2, 1
	.long 0, 0, 0, 0
com3_state:
	.hword COM3, 2
	.long 0, 0, 0, 0
com4_state:
	.hword COM4, 3
	.long 0, 0, 0, 0

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
# Clear the PIC's interrupt-inhibit flags for IRQ 3 and IRQ 4.
	inb $PIC1_DATA, %al
	andb $0xE7, %al
	outb %al, $PIC1_DATA
	ret

_uart_transmit:
# parameters: port number, source buffer, byte count
	mov 4(%esp), %eax
	imul $PORT_STATE_SIZE, %ax
	add $port_state, %eax
# Assume for the moment no TX is currently occuring - check that someday.
# We may be interrupted so write fields in an order which won't confuse the
# interrupt handler: zero the head, then write tail first and then head.
	xorl %ecx, %ecx
	movl %ecx, TXHEAD(%eax)
	movl 8(%esp), %ecx
	movl %ecx, TXTAIL(%eax)
	movl %ecx, TXHEAD(%eax)
	movl 12(%esp), %ecx
	addl %ecx, TXTAIL(%eax)
# Enable THRE interrupts. Setting this flag should kick one off immediately.
	xorl %edx, %edx
	movw ADDR(%eax), %dx
	add $IER, %edx
	movb $(IER_RBRI|IER_THRI|IER_LSI|IER_MSI), %al
	out %al, %dx
	ret

config_port:
# Port struct is in EAX and we must save/restore registers as usual.
	pushl %ebp
	movl %eax, %ebp
	pushl %ebx
	xorl %ebx, %ebx
	movw ADDR(%eax), %bx
# Try to put the device in loopback mode, then check its modem state.
	lea MCR(%ebx), %edx
	movb $MCR_LOOP, %al
	outb %al, %dx
	lea MSR(%ebx), %edx
	inb %dx, %al
# None of the status bits should be set until we try to set them.
	andb $0xF0, %al
	jnz 0f
# Still using loopback mode, try to change the modem status.
	lea MCR(%ebx), %edx
	movb $(MCR_LOOP|MCR_DTR|MCR_RTS|MCR_OUT1|MCR_OUT2), %al
	outb %al, %dx
	lea MSR(%ebx), %edx
	inb %dx, %al
# All four bits should now be set the way we configured them.
	andb $0xF0, %al
	cmpb $(MSR_CTS|MSR_DSR|MSR_RI|MSR_DCD), %al
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
# Enable DTR so the other device knows we're listening and set OUT2 so the UART
# will signal its interrupts through the PIC.
	lea MCR(%ebx), %edx
	mov $(MCR_DTR|MCR_OUT2), %al
	outb %al, %dx
# Enable interrupts other than THRE, which we'll enable to start transmission.
	lea IER(%ebx), %edx
	mov $(IER_RBRI|IER_LSI|IER_MSI), %al
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

_isr_IRQ3:
	pushal
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	mov $com2_state, %ebp
	call service_port
	mov $com4_state, %ebp
	call service_port
	popal
	iret

_isr_IRQ4:
	pushal
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	mov $com1_state, %ebp
	call service_port
	mov $com3_state, %ebp
	call service_port
	popal
	iret

service_port:
# Port state is in EBP. We can trash any register because this function is only
# called from the ISRs, which are obligated to restore all registers anyway.
	testb $PORT_PRESENT, FLAGS(%ebp)
	jz 0f
	xorl %ebx, %ebx
	movw ADDR(%ebp), %bx
check_loop:
# Check IIR for the interrupt condition and respond accordingly. The UART may
# have multiple conditions, so we'll poll IIR until bit 1 is set showing that
# all conditions are clear.
	lea IIR(%ebx), %edx
	inb %dx, %al
	testb $IIR_NONE, %al
	jnz 0f
	and $IIR_ID_MASK, %al
	cmp $IIR_MSI, %al
	je modem_status
	cmp $IIR_THRI, %al
	je transmit_clear
	cmp $IIR_RBRI, %al
	je receive_ready
	cmp $IIR_LSI, %al
	je line_status
0:	ret

modem_status:
	lea MSR(%ebx), %edx
	xorl %eax, %eax
	inb %dx, %al
	pushl %eax
	xorl %eax, %eax
	movb INDEX(%ebp), %al
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
	movb INDEX(%ebp), %al
	pushl %eax
	call _uart_line_status
	add $8, %esp
	jmp check_loop

receive_ready:
	mov RXHEAD(%ebp), %esi
	mov RXTAIL(%ebp), %edi
# if receive buffer still has room, read RBR into buffer
# repeat until receive buffer is out of room or LSR stops indicating RBR
# if port has FIFO and receive buffer has fewer than 16 bytes left, or if
# port does not have FIFO and receive buffer is full, drop RTS/CTS/etc and
# call our event handler; if new buffer arrives, continue running
#   ...
0:	jmp check_loop

transmit_clear:
# Has the other device signalled that we are clear to send?
	mov TXHEAD(%ebp), %esi
	cmp $0, %esi
	jz 0f
# How many bytes remain to be sent?
	mov TXTAIL(%ebp), %ecx
	subl %esi, %ecx
	jz 0f
# If the port has no FIFO, we can write only one byte at a time.
	testb $PORT_HAS_FIFO, FLAGS(%ebp)
	jnz 1f
	xorl %ecx, %ecx
	inc %ecx
# If the port has a FIFO, we can write up to 16 bytes at a time.
1:	cmp $16, %ecx
	jle 2f
	xorl %ecx, %ecx
	inc %ecx
	rol $4, %ecx
# Write as many bytes as we can to the THR, then save the new head pointer.
2:	lea THR(%ebx), %edx
	rep outsb
	movl %esi, TXHEAD(%ebp)
# Did we just empty the send buffer?
# If the buffer has just become empty, we need to disable our THRE interrupt
# and give our client a chance to send more by signalling tx_clear.
	cmpl %esi, TXTAIL(%ebp)
	jne 0f
	lea IER(%ebx), %edx
	movb $(IER_RBRI|IER_LSI|IER_MSI), %al
	outb %al, %dx
	movb INDEX(%ebp), %al
	and $3, %eax
	push %eax
	call _uart_tx_clear
	add $4, %esp
0:	jmp check_loop

