# Configure an IDT descriptor with the address and size of an IDT table,
# then tell the cpu to load it.

.section .data
.align 8
idt_descriptor:
idt_descriptor_limit:
	.hword 1535
idt_descriptor_base:
	.long idt

.section .text
.global _idt_load
.type _idt_load, @function
_idt_load:
	movl 4(%esp), %eax		# table address
	movl %eax, idt_descriptor_base
	movl 8(%esp), %eax
	mull 6					# entry count * size of entry
	decl %eax				# because that's how it works
	movw %ax, idt
	lidtl (idt_descriptor)
	ret

