# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global _start, _startc

.section .text
_start:
	# We'll reserve space in the .data section to use as a call stack.
	movl $stack_top, %esp

	# Save the bootloader info so we can pass it on to _startc.
	pushl %ebx
	pushl %eax

	# The bootloader should have given us a flat 32-bit address space, but we
	# aren't supposed to assume that so we'll install our own GDT anyway.
	lgdtl (gdtr)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x8, $0f; 0:

	# Call the C runtime entrypoint and let the program do its thing.
	call _startc

	# There's no more work to do should that call ever return, so we'll halt,
	# and we'll do so in a loop in case an NMI wakes it back up again.
.Lhang:
	cli
	hlt
	jmp .Lhang

.section .multiboot
.align 4
.set FLAGS, 0x00000003 # use page alignment and provide memory map
.set MAGIC, 0x1BADB002 # bootloader looks for this magic number
.set CHECKSUM, -(MAGIC + FLAGS) # no, the magic number was not accidental
.long MAGIC, FLAGS, CHECKSUM

.section .bootstrap_stack, "aw", @nobits
.skip 16384 # 16 KiB
stack_top:

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

