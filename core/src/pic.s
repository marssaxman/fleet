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
.global _pic_init
.type _pic_init, @function
_pic_init:
	// Start initialization and enable ICW4
	movl $0x11, %eax
	movl $PIC1_CMD, %edx
	outb %al, %dx
	movl $PIC2_CMD, %edx
	outb %al, %dx

	// Set up the vector table offsets
	movl $0x20, %eax
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $0x28, %eax
	movl $PIC2_DATA, %edx
	outb %al, %dx

	// Configure the master/slave wiring
	movl $0x04, %eax
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $0x02, %eax
	movl $PIC2_DATA, %edx
	outb %al, %dx

	// Use 8086 mode and other typical settings
	movl $0x01, %eax
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $PIC2_DATA, %edx
	outb %al, %dx

	// Fall right on through to the disable function
.global _pic_disable
.type _pic_disable, @function
_pic_disable:
	movl $0xFF, %eax
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $PIC2_DATA, %edx
	outb %al, %dx
	ret

.global _pic_enable
.type _pic_enable, @function
_pic_enable:
	movl $0x00, %eax
	movl $PIC1_DATA, %edx
	outb %al, %dx
	movl $PIC2_DATA, %edx
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

