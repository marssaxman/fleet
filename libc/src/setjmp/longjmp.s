# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global longjmp
.type longjmp, @function
longjmp:
	# get the address of the jmp_buf
	mov 4(%esp), %edx
	# get the return value
	mov 8(%esp), %eax
	# value must not be equal to 0
	test %eax, %eax
	cmovz $1, %eax
	# restore the nonvolatile register state
	mov (%edx), %ebx
	mov 4(%edx), %esi
	mov 8(%edx), %edi
	# restore the stack frame
	mov 12(%edx), %ebp
	mov 16(%edx), %ecx
	mov %ecx, %esp
	# jump back to the original return address
	mov 20(%edx), %ecx
	jmp *%ecx
