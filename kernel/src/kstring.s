# Copyright (C) 2015 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

# GCC requires a freestanding environment to provide implementations of
# memcpy, memmove, memset and memcmp, and these are those implementations.
# They are defined as weak symbols with the expectation that the kernel will
# be linked against some libc which will provide better versions.
# The kernel itself can't depend on libc, however, so we export each function
# with a suitable kernel-specific entrypoint name as well.

.text

# void *memmove(void *dest, const void *src, size_t len);
# Copy 'len' bytes from 'src' to 'dest', coping with the likelihood that the
# source and destination memory regions overlap.
memmove: .global memmove; .weak memmove
_kmemmove: .global _kmemmove
	pushl %esi
	pushl %edi
	# load parameter values off the stack and prepare the return value
	movl 12(%esp), %edi		# dest
	movl 16(%esp), %esi		# src
	movl 20(%esp), %ecx		# len
	movl %edi, %eax
	# if dest >= src, jump over to memcpy and do it the easy way
	cmpl %esi, %edi
	jna .L_copy_forward
	# put the processor in backwards mode
	std
	# move to the end of each buffer
	addl %ecx, %edi
	addl %ecx, %esi
	# copy single bytes until we reach 4-byte alignment
	andl $3, %ecx
	decl %edi
	decl %esi
	rep; movsb
	# copy the remaining data in 4-byte chunks
	movl 20(%esp), %ecx
	shrl $2, %ecx
	subl $3, %esi
	subl $3, %edi
	rep; movsl
	# clean up and go home
	cld
	popl %edi
	popl %esi
	ret

# void *memcpy(void *dest, const void *src, size_t len);
# Copy 'len' bytes from 'src' to 'dest', assuming that we don't have to worry
# about overlap.
memcpy: .global memcpy; .weak memcpy
_kmemcpy: .global _kmemcpy
	# save the nonvolatile registers we will use for the string copy
	pushl %esi
	pushl %edi
	# load parameter values off the stack and prepare the return value
	movl 12(%esp), %edi		# dest
	movl 16(%esp), %esi		# src
	movl 20(%esp), %ecx		# len
	movl %edi, %eax
.L_copy_forward:
	# copy 4 bytes at a time, using rep to make the microcode do the work
	shrl $2,%ecx
	cld
	rep; movsl
	# copy any remaining bytes at the end, one at a time
	movl 20(%esp), %ecx
	andl $3, %ecx
	rep; movsb
	# restore registers and return
	popl %edi
	popl %esi
	ret

# void *memset(void *ptr, int value, size_t num);
# Fill the first 'num' bytes of the buffer pointed at by 'ptr' to 'value'.
# This is a bytewise operation despite the value's type being int.
memset: .global memset; .weak memset
_kmemset: .global _kmemset
	pushl %edi
	# load parameters and set up for the loop
	movl 0x08(%esp), %edi
	movzbl 0x0C(%esp), %eax
	movl 0x10(%esp), %ecx
	# hey cpu, mind doing all the work for us?
	cld
	rep; stosb
	# thanks
	movl 0x08(%esp), %eax
	popl %edi
	ret

# int memcmp(const void *ptr1, const void *ptr2, size_t num);
# Compare the first 'num' bytes of the block 'ptr1' points at to the first
# 'num' bytes of the block 'ptr2' points at, returning zero if they all match
# or some <0, >0 value indicating which is greater if they do not match.
memcmp: .global memcmp; .weak memcmp
_kmemcmp: .global _kmemcmp
	pushl %edi
	pushl %esi
	# load parameters into the good ol' string rep registers
	movl 12(%esp), %edi
	movl 16(%esp), %esi
	movl 20(%esp), %ecx
	# Compare 4 bytes at a time. If we make it to the end and all bytes are
	# equal, continue on and check the remaining bytes individually.
	shrl $2, %ecx
	cld
	repe; cmpsl
	jne .L_recheck
	movl 20(%esp), %ecx
	andl $3, %ecx
	jmp .L_comparebytes
.L_recheck:
	# One or more pairs of bytes in the last word did not match. Back up
	# and try it again, bytewise.
	movl $4, %ecx
	subl %ecx, %edi
	subl %ecx, %esi
.L_comparebytes:
	repe; cmpsb
	movzbl -1(%edi), %eax
	movzbl -1(%esi), %edx
	subl %edx, %eax
	popl %esi
	popl %edi
	ret

# void *_kmemzero(void *ptr, size_t num);
# Specialization of memset with value = '\0', the most common usage.
_kmemzero: .global _kmemzero
	pushl %edi
	# load the parameters
	movl 0x08(%esp), %edi
	movl 0x0C(%esp), %ecx
	# stosb/stosl write from %al/%eax to (%edi)
	xorl %eax, %eax
	cld
	# word-align the write pointer
0:	testb $0x03, %cl
	jz 1f
	stosb
	loop 0b
1:	# write as many words as we can
	push %ecx
	shrl $2, %ecx
	test %ecx, %ecx
	jz 2f
	rep; stosl
2:	# if there are bytes remaining, zero them out too
	pop %ecx
	and $0x03, %ecx
	jz 3f
	rep; stosb
	# return the original buffer address
3:	movl 0x08(%esp), %eax
	popl %edi
	ret

