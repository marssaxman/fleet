# Tell the CPU where the new IDT descriptor lives.

.section .data
.align 8
.global _idt
_idt:
	.rept 256
	.hword 0x0000	# offset_low
	.hword 0x0000	# selector
	.byte 0x00		# zero
	.byte 0x00		# flags
	.hword 0x0000	# offset_high
	.endr
.global _idtptr
_idtptr:
	.hword 0x07FF	# size of table - 1
	.long _idt 		# address

.section .text
.global _idt_init
_idt_init:
	lidtl _idtptr
	ret

.global _idt_load
_idt_load:
	movl 4(%esp), %eax		# descriptor address
	lidtl (%eax)
	ret

