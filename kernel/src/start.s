# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

	.section .multiboot
	.align 4
	.set FLAGS, 0x00000003 # use page alignment and provide memory map
	.set MAGIC, 0x1BADB002 # bootloader looks for this magic number
	.set CHECKSUM, -(MAGIC + FLAGS) # no, the magic number was not accidental
	.long MAGIC, FLAGS, CHECKSUM
	.section .text

_start: .global _start
	# Make sure EAX contains the multiboot magic code, because we depend on the
	# multiboot information record which should be in EBX.
	cmpl $0x2BADB002, %eax
	jne .Lhang
	# Make sure the multiboot info has lower/upper memory information.
	testb $1, (%ebx)
	jz .Lhang
	# This is a single-stack kernel, so we'll place the base of the stack at the
	# end of upper memory. Upper memory starts at 0x00100000, and the multiboot
	# info record contains the number of 1K blocks it contains.
	movl $0x00100000, %esp
	movl 8(%ebx), %eax
	rol $10, %eax
	addl %eax, %esp

	# The bootloader should have given us a flat 32-bit address space, but we
	# can't make any assumptions about the GDT, so we'll install our own.
	.section .descriptor
	.align 8
gdt:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 # null
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00 # system code
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00 # system data
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFA, 0xCF, 0x00 # user code
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF2, 0xCF, 0x00 # user data
	.set GDT_SIZE, .-gdt
	.section .data
gdtr:
	.hword GDT_SIZE-1
	.long gdt
	.section .text
	lgdtl (gdtr)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x8, $0f; 0:

	# Set up an interrupt table. We'll create an entry for each interrupt, with
	# a wrapper that invokes _exception for the first 32 vectors and _interrupt
	# for the rest.
	.set has_code_mask, 0
	.irp index, 0x08,0x0A,0x0B,0x0C,0x0D,0x0E,0x11
		.set has_code_mask, has_code_mask|(1<<\index)
	.endr
	.set ISR_THUNK_SIZE, 16
	.global _interrupt
	.section .interrupt
	.align ISR_THUNK_SIZE
isr_thunk_table:
	.macro gate
	# Allocate the 8-byte gate entry descriptor.
	.hword 0 # offset low
	.hword 8 # segment selector from the GDT
	.byte 0 # unused
	.byte 0x8E # 32-bit interrupt gate present
	.hword 0 # offset high
	# Generate the entrypoint function, which will set up the stack and
	# call a C function corresponding to this interrupt.
	.section .interrupt
isr\@ :
	pushal
	push $\@
	.global _interrupt\@
	.weakref _interrupt\@, _interrupt
	call _interrupt\@
	add $4, %esp
	popal
	.ifgt \@-32; .if \@&has_code_mask
		add $4, %esp
	.endif; .endif
	iret
	.ifgt .-isr\@-ISR_THUNK_SIZE; .err; .endif
	.align ISR_THUNK_SIZE
	.section .descriptor
	.endm
	.set VECTOR_COUNT, 256
	.section .descriptor
	.align 8
idt:
	.rept VECTOR_COUNT; gate; .endr
	.set IDT_SIZE, .-idt
	.section .data
idtr:
	.hword IDT_SIZE-1
	.long idt
	.section .text
	movl $isr_thunk_table, %eax
	movl $idt, %edx
	xorl %ecx, %ecx
	notb %cl
set_gate:
	movw %ax, (%edx)
	ror $16, %eax
	movw %ax, 6(%edx)
	ror $16, %eax
	add $ISR_THUNK_SIZE, %eax
	add $8, %edx
	loop set_gate
	lidtl (idtr)

	# Call the kernel entrypoint, passing in the multiboot info.
	pushl %ebx
	call _kernel

	# The program is done. Halt, and halt in a loop in case an NMI wakes us up.
.Lhang:
	cli
	hlt
	jmp .Lhang


