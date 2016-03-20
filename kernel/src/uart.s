# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _isr_cpu03, _isr_cpu04

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

# COM2 and COM4 share IRQ3.
_isr_cpu03:
	pushal
	cld

	push $8
	push $FLOO
	call _console_write

	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	popal
	iret

# COM1 and COM3 share IRQ4.
_isr_cpu04:
	pushal
	cld

	push $8
	push $DLEB
	call _console_write

	movb $PIC_EOI, %al
	outb %al, $PIC1_CMD
	popal
	iret



.section .data
FLOO: .ascii "YO MOMMA";
DLEB: .ascii "SPRIBBLE";
