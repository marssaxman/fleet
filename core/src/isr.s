# Here we find 256 interrupt service routine stubs which generate a common
# stack layout and invoke the master ISR. We align these stubs at 16-byte
# intervals so they can be indexed easily without having to know exactly how
# large each one happens to be.

.macro stub index
	push $0
	push $\index
	jmp common
	.align 16
.endm

.macro coded_stub index
	push $\index
	jmp common
	.align 16
.endm

.macro stub_sequence index count
	.if \count
		stub \index
		stub_sequence "(\index+1)", "(\count-1)"
	.endif
.endm

.align 16, 0x90
.global _isr_stubs
_isr_stubs:
	stub_sequence index=0, count=8
	coded_stub 8
	stub 9
	coded_stub 10
	coded_stub 11
	coded_stub 12
	coded_stub 13
	coded_stub 14
	stub_sequence index=15, count=2
	coded_stub 17
	stub_sequence index=18, count=12
	coded_stub 30
	stub 31
	stub_sequence index=32, count=33
	stub_sequence index=64, count=32
	stub_sequence index=96, count=32

# The master ISR will be defined in some C module.
.global _isr
.type _isr, @function
common:
	pushal				# save all registers
	movl %esp, %ebp		# set up the stack frame chain
	cld					# voodoo - SysV calling conventions?
	pushl %esp			# pointer to the saved register data on the stack
	call _isr			# let the C world handle things
	addl 0xC, %esp		# clear off data ptr, vector number, and error code
	popal				# restore registers from stack
	iret				# return from interrupt state

