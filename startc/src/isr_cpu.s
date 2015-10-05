# ISR entrypoint stubs for CPU exception interrupts.

.macro isr_cpu id
	pushal
	push $\id
	jmp common_exception
.endm

.global _isr_cpu00
_isr_cpu00:
	push $0
	isr_cpu 0x00

.global _isr_cpu01
_isr_cpu01:
	push $0
	isr_cpu 0x01

.global _isr_cpu02
_isr_cpu02:
	push $0
	isr_cpu 0x02

.global _isr_cpu03
_isr_cpu03:
	push $0
	isr_cpu 0x03

.global _isr_cpu04
_isr_cpu04:
	push $0
	isr_cpu 0x04

.global _isr_cpu05
_isr_cpu05:
	push $0
	isr_cpu 0x05

.global _isr_cpu06
_isr_cpu06:
	push $0
	isr_cpu 0x06

.global _isr_cpu07
_isr_cpu07:
	push $0
	isr_cpu 0x07

.global _isr_cpu08
_isr_cpu08:
	isr_cpu 0x08

.global _isr_cpu09
_isr_cpu09:
	push $0
	isr_cpu 0x09

.global _isr_cpu0A
_isr_cpu0A:
	isr_cpu 0x0A

.global _isr_cpu0B
_isr_cpu0B:
	isr_cpu 0x0B

.global _isr_cpu0C
_isr_cpu0C:
	isr_cpu 0x0C

.global _isr_cpu0D
_isr_cpu0D:
	isr_cpu 0x0D

.global _isr_cpu0E
_isr_cpu0E:
	isr_cpu 0x0E

.global _isr_cpu0F
_isr_cpu0F:
	push $0
	isr_cpu 0x0F

.global _isr_cpu10
_isr_cpu10:
	push $0
	isr_cpu 0x10

.global _isr_cpu11
_isr_cpu11:
	isr_cpu 0x11

.global _isr_cpu12
_isr_cpu12:
	push $0
	isr_cpu 0x12

.global _isr_cpu13
_isr_cpu13:
	push $0
	isr_cpu 0x13

.global _isr_cpu
.type _isr_cpu, @function
common_exception:
	cld					# make sure we're not in backward string mode
	popl %eax			# retrieve the exception number
	pushl %esp			# parameter: address of saved state
	pushl %eax			# parameter: exception number
	call _isr_cpu		# let the C world handle things
	addl $0x08, %esp	# remove parameters
	popal				# restore register state
	addl $0x04, %esp	# remove error code
	iret				# return from interrupt state

