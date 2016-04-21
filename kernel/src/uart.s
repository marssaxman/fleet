# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# external entrypoints we invoke
.global _uart_isr_thre, _uart_isr_rbr, _uart_isr_lsi, _uart_isr_msi

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

# Field offsets for the state struct
.set ADDR, 0x0 # short
.set TX_BASE, ADDR+4 # ptr
.set TX_SIZE, TX_BASE+4 # int
.set RX_BASE, TX_SIZE+4 # ptr
.set RX_SIZE, RX_BASE+4 # int

.macro prolog
	pushl %ebx
	xorl %ebx, %ebx
	movw 0x8(%esp), %bx
.endm

.macro epilog
	popl %ebx
	ret
.endm

.macro write name
	lea \name(%ebx), %edx
	outb %al, %dx
.endm

.macro read name
	lea \name(%ebx), %edx
	inb %dx, %al
.endm

.section .text

# int _uart_probe(uint16_t addr);
# We have reason to believe that there may be a UART device at this address.
# Try to communicate with it: if its responses are consistent with a UART,
# configure it with reasonable defaults and populate the device data struct.
_uart_probe: .global _uart_probe
	prolog
	xorl %eax, %eax
	# Put the device in loopback mode and clear all of its modem status bits.
	# Check the MSR to see if its state is consistent with our configuration.
	movb $MCR_LOOP, %al
	write MCR
	read MSR
	testb $(MSR_CTS|MSR_DSR|MSR_RI|MSR_DCD), %al
	setnz %al
	jnz .L_probe_return
	# Verify that the success was not an accident by setting all four status
	# bits, then checking MSR. The corresponding flags should now be set.
	movb $(MCR_LOOP|MCR_DTR|MCR_RTS|MCR_OUT1|MCR_OUT2), %al
	write MCR
	read MSR
	testb $(MSR_CTS|MSR_DSR|MSR_RI|MSR_DCD), %al
	setnz %al
	jz .L_probe_return
	# This appears to be a working UART. Give it some reasonable settings.
	# Put the port into DLAB mode so we can set its speed. Use 115.2K, because
	# fastest is best, and because we are actually talking to an emulated UART
	# in the host machine so there's no reason to hold back.
	mov $0x80, %al
	write LCR
	xor %al, %al
	write DLH
	inc %al
	write DLL
	# Clear DLAB, since its job is done, and configure the standard 8N1 mode.
	# Both changes are accomplished by writing to LCR.
	mov $0x03, %al
	write LCR
	# Leave the modem state cleared and interrupts disabled; we'll enable them
	# when the port is opened.
	xorl %eax, %eax
	write IER
	write MCR
	# Attempt to enable FIFO mode for better performance. We expect the device
	# to be an emulated 16550A, but we'll check the IIR flag bits anyway to
	# be sure the FIFO is present and functional, because it's tidier that way.
	mov $0xE7, %al
	write FCR
	read IIR
	testb $IIR_FIFO, %al
	setz %al
	jnz .L_probe_return
	# FIFO configuration failed. Turn FIFO mode back off, since we don't know
	# how this older device will respond.
	xorl %eax, %eax
	write FCR
.L_probe_return:
	epilog

# void _uart_open(uint16_t addr);
# Raise DTR and enable PIC interrupts. Enable RBRI, LSI, and MSI so we get
# notifications if the other end of the line does something interesting.
_uart_open: .global _uart_open
	prolog
	movb $(MCR_DTR|MCR_OUT2), %al
	write MCR
	mov $(IER_RBRI|IER_LSI|IER_MSI), %al
	write IER
	epilog

# void _uart_close(uint16_t addr);
# Drop DTR and RTS to signal the other device we are no longer active.
# Disable interrupts and shut down our transmit/receive loops.
_uart_close: .global _uart_close
	prolog
	xorl %eax, %eax
	write MCR
	write IER
	epilog

# void _uart_tx_start(uint16_t addr);
# The THRE interrupt controls our transmit loop. If transmission is active,
# setting THRE will have no effect; but if it was not previously set, setting
# it will trigger an immediate THRI, to which we can respond by sending data.
_uart_tx_start: .global _uart_tx_start
	prolog
	read IER
	orb $IER_THRI, %al
	outb %al, %dx
	epilog

# void _uart_tx_stop(uint16_t addr);
# Clearing THRE instructs the UART to stop notifying us when it is ready to
# receive more data. The THRI drives our transmit loop, so we'll get no more
# notifications prompting us to send bytes.
_uart_tx_stop: .global _uart_tx_stop
	prolog
	read IER
	andb $(~IER_THRI), %al
	outb %al, %dx
	epilog

# void _uart_rx_start(uint16_t addr);
# The RTS line controls our receive loop. Raising it gives the remote device
# permission to send. We leave the RBR interrupt enabled all the time anyway,
# but it won't fire until we've actually received some data.
_uart_rx_start: .global _uart_rx_start
	prolog
	read MCR
	orb $MCR_RTS, %al
	outb %al, %dx
	epilog

# void _uart_rx_stop(uint16_t addr);
# Dropping RTS signals the remote device that we are no longer interested in
# receiving data, which will hopefully stop the incoming flow. We'll leave the
# RBR interrupt enabled in case there is already data in the pipeline.
_uart_rx_stop: .global _uart_rx_stop
	prolog
	read MCR
	andb $(~MCR_RTS), %al
	outb %al, %dx
	epilog

# void _uart_service(struct uart_state*);
# An event has occurred which may indicate work to be done on this device.
# Check IIR for the interrupt condition and respond accordingly. The UART may
# have multiple conditions, so we'll poll IIR until bit 1 is set showing that
# all conditions are clear.
_uart_service: .global _uart_service
	pushl %ebx
	pushl %ebp
	movl 0x0C(%esp), %ebp # port state
	xorl %ebx, %ebx
	movw ADDR(%ebp), %bx
.L_check_loop:
	read IIR
	testb $IIR_NONE, %al
	jnz .L_none
	and $IIR_ID_MASK, %al
	cmp $IIR_MSI, %al
	je .L_modem_status
	cmp $IIR_THRI, %al
	je .L_transmit_clear
	cmp $IIR_RBRI, %al
	je .L_receive_ready
	cmp $IIR_LSI, %al
	je .L_line_status
.L_none:
	popl %ebp
	popl %ebx
	ret
.L_modem_status:
	xorl %eax, %eax
	read MSR
	push %eax
	push %ebp
	call _uart_isr_msi
	add $8, %esp
	jmp .L_check_loop
.L_line_status:
	xorl %eax, %eax
	read LSR
	push %eax
	push %ebp
	call _uart_isr_lsi
	add $8, %esp
	jmp .L_check_loop
.L_receive_ready:
	# The port has signalled RBR, suggesting that we could receive data.
	# Do we have any empty space waiting in our receive buffer?
	mov RX_SIZE(%ebp), %ecx
	testl %ecx, %ecx
	je .L_rx_full
	# Read bytes until we fill the buffer or drain the UART's queue.
	cld
	mov RX_BASE(%ebp), %edi
.L_rx_next:
	read LSR
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
	movb $(MCR_DTR|MCR_OUT2), %al
	write MCR
	jmp .L_check_loop
	# Save the updated buffer address and size now that we've consumed some.
.L_rx_wait:
	movl %edi, RX_BASE(%ebp)
	movl %ecx, RX_SIZE(%ebp)
	jmp .L_check_loop
.L_transmit_clear:
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
	read LSR
	testb $LSR_THRE, %al
	jz .L_tx_wait
	read MSR
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
	movb $(IER_RBRI|IER_LSI|IER_MSI), %al
	write IER
	jmp .L_check_loop
.L_tx_wait:
	movl %esi, TX_BASE(%ebp)
	movl %ecx, TX_SIZE(%ebp)
	jmp .L_check_loop

