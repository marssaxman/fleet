// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# Stubs for CPU control activities inaccessible from C.

.section .text

.global _outb
.type _outb, @function
_outb:
	movl 4(%esp), %edx	# port address
	movl 8(%esp), %eax	# value to poke
	outb %al, %dx
	ret

.global _inb
.type _inb, @function
_inb:
	movl 4(%esp), %edx	# port address
	xorl %eax, %eax
	inb %dx, %al
	ret

.global _outl
.type _outl, @function
_outl:
	movl 4(%esp), %edx	# port address
	movl 8(%esp), %eax	# value to poke
	outl %eax, %dx
	ret

.global _inl
.type _inl, @function
_inl:
	movl 4(%esp), %edx	# port address
	inl %dx, %eax
	ret

