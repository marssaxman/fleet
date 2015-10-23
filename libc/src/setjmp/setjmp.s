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
