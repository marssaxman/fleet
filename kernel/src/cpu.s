# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.set CONFIG_PORT_A, 0x0092
.set RESET_FLAG, 1

.section .text

_cpu_int_enable: .global _cpu_int_enable
	sti
	ret

_cpu_int_disable: .global _cpu_int_disable
	cli
	ret

_cpu_halt: .global _cpu_halt
	hlt
	ret

_cpu_reset: .global _cpu_reset
	inb $CONFIG_PORT_A, %al
	andb $~RESET_FLAG, %al
	outb %al, $CONFIG_PORT_A
	orb $RESET_FLAG, %al
	outb %al, $CONFIG_PORT_A

