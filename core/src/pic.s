# Initialize the programmable interrupt controller chip.
# By default, all hardware IRQs are mapped onto vectors used for processor
# exceptions, which makes it sort of difficult to figure out what's going on
# when we land in an interrupt handler. We will remap the IRQs above the
# exception slots instead.

.set PIC1_CMD, 0x0020
.set PIC1_DATA, 0x0021
.set PIC2_CMD, 0x00a0
.set PIC2_DATA, 0x00a1

.section .text
.global _pic_set_irqs
.type _pic_set_irqs, @function
_pic_set_irqs:
	// The stack parameter is a bitmask for both PICs, with bits set for
	// the IRQs we want to receive. The PICs are more interested in knowing
	// which IRQs we want to suppress, so we'll invert the bits.
	movb 4(%esp), %al
	xor $0xFF, %al
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $PIC2_DATA, %edx
	movb 5(%esp), %al
	xor $0xFF, %al
	outb %al, %dx
	ret

.global _pic_eoi_slave
.type _pic_eoi_slave, @function
_pic_eoi_slave:
	movl $0x20, %eax # EOI command
	movl $PIC2_CMD, %edx
	outb %al, %dx
.global _pic_eoi_master
.type _pic_eoi_master, @function
_pic_eoi_master:
	movl $0x20, %eax # EOI command
	movl $PIC1_CMD, %edx
	outb %al, %dx
	ret

