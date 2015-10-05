# These interrupt handlers respond to device-initiated IRQs.
.section .text
.global _isr_irq0
_isr_irq0:
	pushal
	push $0x00
	jmp _common_irq_master
.global _isr_irq1
_isr_irq1:
	pushal
	push $0x01
	jmp _common_irq_master
.global _isr_irq2
_isr_irq2:
	pushal
	push $0x02
	jmp _common_irq_master
.global _isr_irq3
_isr_irq3:
	pushal
	push $0x03
	jmp _common_irq_master
.global _isr_irq4
_isr_irq4:
	pushal
	push $0x04
	jmp _common_irq_master
.global _isr_irq5
_isr_irq5:
	pushal
	push $0x05
	jmp _common_irq_master
.global _isr_irq6
_isr_irq6:
	pushal
	push $0x06
	jmp _common_irq_master
.global _isr_irq7
_isr_irq7:
	pushal
	push $0x07
	jmp _common_irq_master
.global _isr_irq8
_isr_irq8:
	pushal
	push $0x08
	jmp _common_irq_slave
.global _isr_irq9
_isr_irq9:
	pushal
	push $0x09
	jmp _common_irq_slave
.global _isr_irqA
_isr_irqA:
	pushal
	push $0x0A
	jmp _common_irq_slave
.global _isr_irqB
_isr_irqB:
	pushal
	push $0x0B
	jmp _common_irq_slave
.global _isr_irqC
_isr_irqC:
	pushal
	push $0x0C
	jmp _common_irq_slave
.global _isr_irqD
_isr_irqD:
	pushal
	push $0x0D
	jmp _common_irq_slave
.global _isr_irqE
_isr_irqE:
	pushal
	push $0x0E
	jmp _common_irq_slave
.global _isr_irqF
_isr_irqF:
	pushal
	push $0x0F
	jmp _common_irq_slave

.global _isr_irq
.type _isr_irq, @function

.global _pic_eoi_master
.type _pic_eoi_master, @function
_common_irq_master:
	cld
	call _isr_irq
	pop %eax
	call _pic_eoi_master
	popal
	iret

.global _pic_eoi_slave
.type _pic_eoi_slave, @function
_common_irq_slave:
	cld
	call _isr_irq
	pop %eax
	call _pic_eoi_slave
	popal
	iret




