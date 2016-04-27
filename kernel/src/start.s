# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _start, _kernel

.section .text
_start:
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
# aren't supposed to assume that, so we'll install our own GDT anyway.
	lgdtl (gdtr)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x8, $0f; 0:

# Call the kernel entrypoint, passing in the multiboot info.
	pushl %ebx
	call _kernel

# The program is done. Halt, and halt in a loop in case an NMI wakes us up.
.Lhang:
	.set CONFIG_PORT_A, 0x0092
	.set RESET_FLAG, 1
	cli
	inb $CONFIG_PORT_A, %al
	andb $~RESET_FLAG, %al
	outb %al, $CONFIG_PORT_A
	orb $RESET_FLAG, %al
	outb %al, $CONFIG_PORT_A
	hlt
	jmp .Lhang

.section .multiboot
.align 4
.set FLAGS, 0x00000003 # use page alignment and provide memory map
.set MAGIC, 0x1BADB002 # bootloader looks for this magic number
.set CHECKSUM, -(MAGIC + FLAGS) # no, the magic number was not accidental
.long MAGIC, FLAGS, CHECKSUM

.section .data
.align 8
gdt:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 # null
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00 # system code
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00 # system data
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFA, 0xCF, 0x00 # user code
	.byte 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xF2, 0xCF, 0x00 # user data
gdtr:
	.hword 39	# limit: size of table in bytes, minus one
	.long gdt	# base: address of table

