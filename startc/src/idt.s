# Set up an IDT and configure its selectors.
# We will configure the interrupts corresponding to CPU exceptions and to IRQs,
# after reconfiguring the PICs to move them up to 0x20. Other interrupts will
# be left blank. We will use segment 0x08 as defined in the GDT and gates will
# be configured as interrupts, not traps.

.macro idt_entry selector, flags
	.hword 0x0000	# offset_low
	.hword \selector
	.byte 0x00		# zero
	.byte \flags
	.hword 0x0000	# offset_high
.endm
.macro active_entries count
	.rept \count
	idt_entry 0x0008, flags=0x8E
	.endr
.endm
.macro blank_entries count
	.rept \count
	idt_entry 0x0000, flags=0x00
	.endr
.endm

.section .data
.align 8
.global _idt
_idt:
	active_entries 0x14	# 0x00..0x13: CPU exceptions
	blank_entries 0x0C  # 0x14..0x1F: reserved but unused
	active_entries 0x10 # 0x20..0x2F: remapped IRQs
	blank_entries 0xD0  # 0x30..0xFF: unused
.global _idtr
_idtr:
	.hword 0x07FF	# size of table - 1
	.long _idt 		# address

.macro setgate index, target
	.set offset, \index << 3	# IDT entry size = 8
	movl $\target, %eax
	movw %ax, _idt + offset + 0
	shrl $16, %eax
	movw %ax, _idt + offset + 6
.endm

.section .text
.global _idt_init
_idt_init:
	setgate 0x00, _isr_cpu00
	setgate 0x01, _isr_cpu01
	setgate 0x02, _isr_cpu02
	setgate 0x03, _isr_cpu03
	setgate 0x04, _isr_cpu04
	setgate 0x05, _isr_cpu05
	setgate 0x06, _isr_cpu06
	setgate 0x07, _isr_cpu07
	setgate 0x08, _isr_cpu08
	setgate 0x09, _isr_cpu09
	setgate 0x0A, _isr_cpu0A
	setgate 0x0B, _isr_cpu0B
	setgate 0x0C, _isr_cpu0C
	setgate 0x0D, _isr_cpu0D
	setgate 0x0E, _isr_cpu0E
	setgate 0x0F, _isr_cpu0F
	setgate 0x10, _isr_cpu10
	setgate 0x11, _isr_cpu11
	setgate 0x12, _isr_cpu12
	setgate 0x13, _isr_cpu13
	lidtl _idtr
	ret

