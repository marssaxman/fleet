#ifndef INTERRUPT_ISR_H
#define INTERRUPT_ISR_H

// The ISR stubs will push these values onto the stack:
struct isr_parameter_registers
{
	// Register state saved by pushal
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	// Interrupt number pushed by our stub
	uint32_t interrupt_number;
	// Error code may have been pushed by the processor or by our stub
	uint32_t error_code;
	// Automatically saved onto the stack by the processor when interrupting
	uint32_t eip, cs, eflags, useresp, ss;
};
// Once the stack is set up the stubs will call this C function:
// void interrupt_handler(struct isr_parameter_registers); 

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

#endif //INTERRUPT_ISR_H
