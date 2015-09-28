# Interrupt service routine stubs
	.text

.global isr0
.type isr0, @function
isr0:
    push 0	# dummy error code
    push 0	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr1
.type isr1, @function
isr1:
    push 0	# dummy error code
    push 1	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr2
.type isr2, @function
isr2:
    push 0	# dummy error code
    push 2	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr3
.type isr3, @function
isr3:
    push 0	# dummy error code
    push 3	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr4
.type isr4, @function
isr4:
    push 0	# dummy error code
    push 4	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr5
.type isr5, @function
isr5:
    push 0	# dummy error code
    push 5	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr6
.type isr6, @function
isr6:
    push 0	# dummy error code
    push 6	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr7
.type isr7, @function
isr7:
    push 0	# dummy error code
    push 7	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr8
.type isr8, @function
isr8: # has an error code
    push 8	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr9
.type isr9, @function
isr9:
    push 0	# dummy error code
    push 9	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr10
.type isr10, @function
isr10: # has an error code
    push 10	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr11
.type isr11, @function
isr11: # has an error code
    push 11	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr12
.type isr12, @function
isr12: # has an error code
    push 12	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr13
.type isr13, @function
isr13: # has an error code
    pushal	# save all registers
    cld
    push 13	# interrupt number
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr14
.type isr14, @function
isr14: # has an error code
    push 14	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr15
.type isr15, @function
isr15:
    push 0	# dummy error code
    push 15	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr16
.type isr16, @function
isr16:
    push 0	# dummy error code
    push 16	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr17
.type isr17, @function
isr17:
    push 0	# dummy error code
    push 17	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr18
.type isr18, @function
isr18:
    push 0	# dummy error code
    push 18	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr19
.type isr19, @function
isr19:
    push 0	# dummy error code
    push 19	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr20
.type isr20, @function
isr20:
    push 0	# dummy error code
    push 20	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr21
.type isr21, @function
isr21:
    push 0	# dummy error code
    push 21	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr22
.type isr22, @function
isr22:
    push 0	# dummy error code
    push 22	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr23
.type isr23, @function
isr23:
    push 0	# dummy error code
    push 23	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr24
.type isr24, @function
isr24:
    push 0	# dummy error code
    push 24	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr25
.type isr25, @function
isr25:
    push 0	# dummy error code
    push 25	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr26
.type isr26, @function
isr26:
    push 0	# dummy error code
    push 26	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr27
.type isr27, @function
isr27:
    push 0	# dummy error code
    push 27	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr28
.type isr28, @function
isr28:
    push 0	# dummy error code
    push 28	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr29
.type isr29, @function
isr29:
    push 0	# dummy error code
    push 29	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr30
.type isr30, @function
isr30:
    push 0	# dummy error code
    push 30	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

.global isr31
.type isr31, @function
isr31:
    push 0	# dummy error code
    push 31	# interrupt number
    pushal	# save all registers
    cld
    call interrupt_handler
    popal	# restore all registers
	add %esp, 8	# clean up the error code and interrupt number
    iret	# return to normal operation

