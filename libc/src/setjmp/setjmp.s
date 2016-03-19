# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global setjmp
.type setjmp, @function
setjmp:
	# get address of jmp_buf
	mov 4(%esp), %eax
	# store values of nonvolatile registers
	mov %ebx, (%eax)
	mov %esi, 4(%eax)
	mov %edi, 8(%eax)
	# store the stack frame and previous SP
	mov %ebp, 12(%eax)
	lea 4(%esp), %ecx
	mov %ecx, 16(%eax)
	# store the return address
	mov (%esp), %ecx
	mov %ecx, 20(%eax)
	# initial return value must be zero
	xor %eax, %eax
	ret
