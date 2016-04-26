# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.section .data

.align 8
.L_idt:
	.rept 48
		.hword 0, 8, 0x8E00, 0
	.endr
.L_idt_end:
.L_idtr:
	.hword .L_idt_end - .L_idt - 1
	.long .L_idt

.section .text

# void _idt_init();
_idt_init: .global _idt_init
	lidtl (.L_idtr)
	ret

# void _idt_config(uint8_t gate, void(*proc)());
_idt_config: .global _idt_config
	movzbl 4(%esp), %ecx
	movl 8(%esp), %eax
	leal .L_idt(,%ecx,8), %edx
	movw %ax, 0(%edx)
	shr $16, %eax
	movw %ax, 6(%edx)
	ret

