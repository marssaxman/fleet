# Here we find interrupt service routine stubs which generate a common stack
# layout and invoke the C entrypoint for handling exceptions.

.global _isr_exc00
_isr_exc00:
	push $0
	push $0x00
	jmp common_exception

.global _isr_exc01
_isr_exc01:
	push $0
	push $0x01
	jmp common_exception

.global _isr_exc02
_isr_exc02:
	push $0
	push $0x02
	jmp common_exception

.global _isr_exc03
_isr_exc03:
	push $0
	push $0x03
	jmp common_exception

.global _isr_exc04
_isr_exc04:
	push $0
	push $0x04
	jmp common_exception

.global _isr_exc05
_isr_exc05:
	push $0
	push $0x05
	jmp common_exception

.global _isr_exc06
_isr_exc06:
	push $0
	push $0x06
	jmp common_exception

.global _isr_exc07
_isr_exc07:
	push $0
	push $0x07
	jmp common_exception

.global _isr_exc08
_isr_exc08:
	push $0x08
	jmp common_exception

.global _isr_exc09
_isr_exc09:
	push $0
	push $0x09
	jmp common_exception

.global _isr_exc0A
_isr_exc0A:
	push $0x0A
	jmp common_exception

.global _isr_exc0B
_isr_exc0B:
	push $0x0B
	jmp common_exception

.global _isr_exc0C
_isr_exc0C:
	push $0x0C
	jmp common_exception

.global _isr_exc0D
_isr_exc0D:
	push $0x0D
	jmp common_exception

.global _isr_exc0E
_isr_exc0E:
	push $0x0E
	jmp common_exception

.global _isr_exc0F
_isr_exc0F:
	push $0
	push $0x0F
	jmp common_exception

.global _isr_exc10
_isr_exc10:
	push $0
	push $0x10
	jmp common_exception

.global _isr_exc11
_isr_exc11:
	push $0x11
	jmp common_exception

.global _isr_exc12
_isr_exc12:
	push $0
	push $0x12
	jmp common_exception

.global _isr_exc13
_isr_exc13:
	push $0
	push $0x13
	jmp common_exception

# The _exception funtion will be defined in some C module.
.global _exception
.type _exception, @function
common_exception:
	pushal				# save all registers
	cld					# voodoo - SysV calling conventions?
	pushl %esp			# pointer to the saved register data on the stack
	call _exception		# let the C world handle things
	addl $0xC, %esp		# clear off data ptr, vector number, and error code
	popal				# restore registers from stack
	iret				# return from interrupt state

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

.global _irq
.type _irq, @function

.global _pic_eoi_master
.type _pic_eoi_master, @function
_common_irq_master:
	cld
	call _irq
	pop %eax
	call _pic_eoi_master
	popal
	iret

.global _pic_eoi_slave
.type _pic_eoi_slave, @function
_common_irq_slave:
	cld
	call _irq
	pop %eax
	call _pic_eoi_slave
	popal
	iret




