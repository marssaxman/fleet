# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# Hello, world! It's time to set up the machine so we can launch the kernel.
.section .text
.global _start
.type _start, @function
_start:
	# Initialize esp to point at our temporary call stack.
	movl $stack_top, %esp
	# Did we get the multiboot header we were looking for?
	cmp $0x2BADB002, %eax
	jne init
	movl %ebx, _multiboot
init:
	call _gdt_init
	call _idt_init
	call _pic_init
	call _kernel
	cli
.Lhang:
	hlt
	jmp .Lhang
.size _start, . - _start

# Create a multiboot header so grub knows it can load this executable.
# It goes in a section all its own so the linker can place it near the start.
.section .multiboot
.align 4
.set FLAGS, 0x00000003 # use page alignment and provide memory map
.set MAGIC, 0x1BADB002 # bootloader looks for this magic number
.set CHECKSUM, -(MAGIC + FLAGS) # no, the magic number was not accidental
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve space for the bootstrap stack inside our executable image, since we
# will need to use it before we've had a chance to examine the memory map.
.section .bootstrap_stack, "aw", @nobits
stack_bottom: .skip 16384 # 16 KiB
stack_top:

# Create storage for the multiboot info so the kernel can use it later.
.section .data
.align 4
.global _multiboot
_multiboot:
.long 0

