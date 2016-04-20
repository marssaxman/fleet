# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# entrypoints we export
.global _uart_init, _uart_transmit, _uart_receive, _uart_state

# external entrypoints we invoke
.global _uart_isr_thre, _uart_isr_rbr, _uart_isr_lsi, _uart_isr_msi

# external entrypoints we override with non-weak implementations
.global _isr_IRQ3, _isr_IRQ4

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC_EOI, 0x0020

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
.set IIR_FIFO, 0x40 # 16550A or later with FIFO enabled
.set MCR_DTR, 0x01 # Control DTR signal
.set MCR_RTS, 0x02 # Control RTS signal
.set MCR_OUT1, 0x04 # Misc control signal 1
.set MCR_OUT2, 0x08 # Misc control signal 2
.set MCR_LOOP, 0x10 # Loopback mode
.set LSR_DR, 0x01 # Data ready
.set LSR_OE, 0x02 # Overrun error
.set LSR_PE, 0x04 # Parity error
.set LSR_FE, 0x08 # Frame error
.set LSR_BI, 0x10 # Break indicator
.set LSR_THRE, 0x20 # Transmit holding register empty
.set LSR_TEMT, 0x40 # Transmitter empty
.set LSR_RXFE, 0x80 # Error in receiver FIFO
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
.set INDEX, ADDR+2 # byte
.set FLAGS, INDEX+1 # byte
.set TX_BASE, FLAGS+1 # ptr
.set TX_SIZE, TX_BASE+4 # int
.set RX_BASE, TX_SIZE+4 # ptr
.set RX_SIZE, RX_BASE+4 # int
.set PORT_STATE_SIZE, RX_SIZE+4

# Port state flag values
.set PORT_PRESENT, 0x01

.section .data
.local COM1, COM2, COM3, COM4

_uart_state:
COM1: .hword 0x03F8; .byte 0, 0; .long 0, 0, 0, 0
COM2: .hword 0x02F8; .byte 1, 0; .long 0, 0, 0, 0
COM3: .hword 0x03E8; .byte 2, 0; .long 0, 0, 0, 0
COM4: .hword 0x02F8; .byte 3, 0; .long 0, 0, 0, 0

.section .text
.local configure, isr_IRQ3, isr_IRQ4, service

_uart_init:
# Check each possible COM port in turn and configure it if present. We use
# %ebx as the port address parameter for internal routines because we have
# to save and restore %ebx inside an interrupt handler anyway, and this way
# it's easy to load specific register addresses without hitting the stack.
	pushl %ebx
	xorl %ebx, %ebx
	pushl %ebp
	mov $COM1, %ebp
	call configure
	mov $COM2, %ebp
	call configure
	mov $COM3, %ebp
	call configure
	mov $COM4, %ebp
	call configure
	popl %ebp
	popl %ebx
# Clear the PIC's interrupt-inhibit flags for IRQ 3 and IRQ 4.
	inb $PIC1_DATA, %al
	andb $0xE7, %al
	outb %al, $PIC1_DATA
	ret
configure:
# Port struct is in EBP.
	movw ADDR(%ebp), %bx
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
	testb $IIR_FIFO, %al
	jnz 0f
# The FIFO is either broken or absent, so we'll avoid trouble by disabling it.
	lea FCR(%ebx), %edx
	xorl %eax, %eax
	outb %al, %dx
0:	ret

_uart_transmit:
	mov 4(%esp), %eax
# Enable THRE interrupts. If they weren't already enabled, setting this flag
# will kick one off right away.
	movw ADDR(%eax), %dx
	add $IER, %dx
	movb $(IER_RBRI|IER_THRI|IER_LSI|IER_MSI), %al
	outb %al, %dx
	ret

_uart_receive:
	mov 4(%esp), %eax
# Raise our RTS line, giving the remote end permission to transmit. We will
# get an RBR interrupt when data next arrives.
	xorl %edx, %edx
	movw ADDR(%eax), %dx
	add $MCR, %edx
	movb $(MCR_RTS|MCR_DTR|MCR_OUT2), %al
	outb %al, %dx
	ret

_isr_IRQ3:
	pushal
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	mov $COM2, %ebp
	call service
	mov $COM4, %ebp
	call service
	popal
	iret

_isr_IRQ4:
	pushal
	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	mov $COM1, %ebp
	call service
	mov $COM3, %ebp
	call service
	popal
	iret

service:
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
	xorl %eax, %eax
	lea MSR(%ebx), %edx
	inb %dx, %al
	push %eax
	push %ebp
	call _uart_isr_msi
	add $8, %esp
	jmp check_loop

line_status:
	xorl %eax, %eax
	lea LSR(%ebx), %edx
	inb %dx, %al
	push %eax
	push %ebp
	call _uart_isr_lsi
	add $8, %esp
	jmp check_loop

receive_ready:
	# The port has signalled RBR, suggesting that we could receive data.
	# Do we have any empty space waiting in our receive buffer?
	mov RX_SIZE(%ebp), %ecx
	testl %ecx, %ecx
	je .L_rx_full
	# Read bytes until we fill the buffer or drain the UART's queue.
	cld
	mov RX_BASE(%ebp), %edi
.L_rx_next:
	lea LSR(%ebx), %dx
	inb %dx, %al
	testb $LSR_DR, %al
	jz .L_rx_wait
	lea RBR(%ebx), %dx
	insb
	loop .L_rx_next
	movl %ecx, RX_SIZE(%ebp)
	# Let our client know that we need another read buffer.
.L_rx_full:
	push %ebp
	call _uart_isr_rbr
	add $8, %esp
	mov RX_BASE(%ebp), %edi
	mov RX_SIZE(%ebp), %ecx
	testl %ecx, %ecx
	jnz .L_rx_next
	# The read buffer is still empty. Drop RTS so the other device knows we 
	# would like it to stop transmitting now.
	lea MCR(%ebx), %edx
	movb $(MCR_RTS|MCR_OUT2), %al
	outb %al, %dx
	jmp check_loop
	# Save the updated buffer address and size now that we've consumed some.
.L_rx_wait:
	movl %edi, RX_BASE(%ebp)
	movl %ecx, RX_SIZE(%ebp)
	jmp check_loop

transmit_clear:
	# The port has signalled THRE, giving us an opportunity to transmit data.
	# Do we have any outgoing data waiting in our transmit buffer?
	mov TX_SIZE(%ebp), %ecx
	testl %ecx, %ecx
	jz .L_tx_empty
	# Read bytes until we drain the buffer, the other device drops CTS, or the
	# line status says the transmit register is no longer empty.
	cld
	mov TX_BASE(%ebp), %esi
.L_tx_next:
	lea LSR(%ebx), %dx
	inb %dx, %al
	testb $LSR_THRE, %al
	jz .L_tx_wait
	lea MSR(%ebx), %dx
	inb %dx, %al
	testb $MSR_CTS, %al
	jz .L_tx_wait
	lea THR(%ebx), %edx
	outsb
	loop .L_tx_next
	movl %ecx, TX_SIZE(%ebp)
	# We have drained the transmit buffer. Ask our client for another one.
.L_tx_empty:
	push %ebp
	call _uart_isr_thre
	add $4, %esp
	mov TX_BASE(%ebp), %esi
	mov TX_SIZE(%ebp), %ecx
	testl %ecx, %ecx
	jnz .L_tx_next
	# The transmit buffer is still empty. Turn off THRE interrupts.
	lea IER(%ebx), %edx
	movb $(IER_RBRI|IER_LSI|IER_MSI), %al
	outb %al, %dx
	jmp check_loop
.L_tx_wait:
	movl %esi, TX_BASE(%ebp)
	movl %ecx, TX_SIZE(%ebp)
	jmp check_loop

