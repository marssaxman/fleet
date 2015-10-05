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

.section .text
.global _idt_init
_idt_init:
	lidtl _idtr
	ret

