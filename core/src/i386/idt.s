# Tell the CPU where the new IDT descriptor lives.

.section .text
.global _idt_load
.type _idt_load, @function
_idt_load:
	movl 4(%esp), %eax		# descriptor address
	lidtl (%eax)
	ret

