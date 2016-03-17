# Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# Hello, world! It's time to set up the machine so we can launch the kernel.
.section .text
.global _start
_start:
	# Initialize esp to point at our temporary call stack.
	movl $stack_top, %esp
	# Save the bootloader info; the kernel will pick them up as parameters.
	pushl %ebx
	pushl %eax
	call _gdt_init	# Configure memory segmentation (flat)
	call _idt_init	# Set up an interrupt descriptor table
	call _pic_init	# Make the interrupt controllers usable
	call _kernel	# Go to the land of C and never come back.

	# There is nothing more we can do. Instead of wandering into the weeds,
	# we'll sleep in a loop until the user reboots the machine.
	cli
.Lhang:
	hlt
	jmp .Lhang

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
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

