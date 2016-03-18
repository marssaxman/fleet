# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.


# Hello, world! Let's configure this machine then go start up the kernel.
.section .text
_start: .global _start
	# Initialize esp to point at our temporary call stack.
	movl $stack_top, %esp
	# Save the bootloader info; the kernel will pick them up as parameters.
	pushl %ebx
	pushl %eax
	call gdt_init
	call _idt_init	# Set up an interrupt descriptor table
	call _pic_init	# Make the interrupt controllers usable
	call _kernel	# Go to the land of C and never come back.

	# There is nothing more we can do. Instead of wandering into the weeds,
	# we'll sleep in a loop until the user reboots the machine.
	cli
.Lhang:
	hlt
	jmp .Lhang


# Reserve space for the bootstrap stack in our executable image, as a zerofill
# section. We won't know anything about the memory map at startup, but we can
# be sure that this section will exist and won't be in use, because otherwise
# the bootloader would have failed.
.section .bootstrap_stack, "aw", @nobits
.skip 16384 # 16 KiB
stack_top:


# The x86 still has a segmented memory architecture, after all these years, so
# if we want a flat address space we must get one by configuring the GDT.
# Since a flat address space is always the same, we can just bake the bytes in
# below, point the CPU at them, and reload the segment registers.
.section .text
gdt_init:
	lgdtl (gdtr)
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x8, $reload_cs
reload_cs:
	ret

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


# A multiboot header is the signal to grub or some other bootlooader that this
# ordinary-looking ELF file is actually a bootable kernel image.
.section .multiboot
.align 4
.set FLAGS, 0x00000003 # use page alignment and provide memory map
.set MAGIC, 0x1BADB002 # bootloader looks for this magic number
.set CHECKSUM, -(MAGIC + FLAGS) # no, the magic number was not accidental
.long MAGIC
.long FLAGS
.long CHECKSUM

