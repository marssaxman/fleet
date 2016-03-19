// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.section .text
_console_write: .global _console_write
	movl 8(%esp), %ecx		# len
	jz 0f
	pushl %esi
	mov $0x00E9, %dx		# port
	movl 0x8(%esp), %esi	# str
	cld; rep outsb
	popl %esi
0:	ret

_console_putc: .global _console_putc
	movb 4(%esp), %al
	outb %al, $0x00E9
	ret

