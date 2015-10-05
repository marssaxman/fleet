# ISR entrypoint stubs for external device-initiated interrupts.

.macro isr_irq_pic1 id
	pushal
	push $\id
	jmp common_irq_pic1
.endm

.macro isr_irq_pic2 id
	pushal
	push $\id
	jmp common_irq_pic2
.endm


.global _isr_irq0
_isr_irq0:
	isr_irq_pic1 0x00
.global _isr_irq1
_isr_irq1:
	isr_irq_pic1 0x01
.global _isr_irq2
_isr_irq2:
	isr_irq_pic1 0x02
.global _isr_irq3
_isr_irq3:
	isr_irq_pic1 0x03
.global _isr_irq4
_isr_irq4:
	isr_irq_pic1 0x04
.global _isr_irq5
_isr_irq5:
	isr_irq_pic1 0x05
.global _isr_irq6
_isr_irq6:
	isr_irq_pic1 0x06
.global _isr_irq7
_isr_irq7:
	isr_irq_pic1 0x07
.global _isr_irq8
_isr_irq8:
	isr_irq_pic2 0x08
.global _isr_irq9
_isr_irq9:
	isr_irq_pic2 0x09
.global _isr_irqA
_isr_irqA:
	isr_irq_pic2 0x0A
.global _isr_irqB
_isr_irqB:
	isr_irq_pic2 0x0B
.global _isr_irqC
_isr_irqC:
	isr_irq_pic2 0x0C
.global _isr_irqD
_isr_irqD:
	isr_irq_pic2 0x0D
.global _isr_irqE
_isr_irqE:
	isr_irq_pic2 0x0E
.global _isr_irqF
_isr_irqF:
	isr_irq_pic2 0x0F

.global _isr_irq
.type _isr_irq, @function

.global _pic_eoi_master
.type _pic_eoi_master, @function
common_irq_pic1:
	cld
	call _isr_irq
	pop %eax
	call _pic_eoi_master
	popal
	iret

.global _pic_eoi_slave
.type _pic_eoi_slave, @function
common_irq_pic2:
	cld
	call _isr_irq
	pop %eax
	call _pic_eoi_slave
	popal
	iret




