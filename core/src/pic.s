# utility function to configure the interrupt controller's IRQ suppression bits

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

