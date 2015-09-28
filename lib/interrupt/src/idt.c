#include "idt.h"
#include <string.h>
#include <stdint.h>
#include "isr.h"

struct idt_entry
{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr_struct
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

#define IDT_SIZE 256
static struct idt_entry idt[IDT_SIZE];

extern void idt_load(void*);

static void idt_set_gate(uint8_t num, void *proc, uint16_t sel, uint8_t flags)
{
	struct idt_entry *gate = &idt[num];
	uint32_t base = (uint32_t)proc;
	gate->offset_low = base & 0x0000FFFF;
	gate->offset_high = (base >> 16) & 0x0000FFFF;
	gate->selector = sel;
	gate->zero = 0;
	gate->flags = flags;
}

void idt_init()
{
	memset(&idt, 0, sizeof(struct idt_entry) * IDT_SIZE);
	idt_set_gate(0x00, isr0, 0x08, 0x8E);
	idt_set_gate(0x01, isr1, 0x08, 0x8E);
	idt_set_gate(0x02, isr2, 0x08, 0x8E);
	idt_set_gate(0x03, isr3, 0x08, 0x8E);
	idt_set_gate(0x04, isr4, 0x08, 0x8E);
	idt_set_gate(0x05, isr5, 0x08, 0x8E);
	idt_set_gate(0x06, isr6, 0x08, 0x8E);
	idt_set_gate(0x07, isr7, 0x08, 0x8E);
	idt_set_gate(0x08, isr8, 0x08, 0x8E);
	idt_set_gate(0x09, isr9, 0x08, 0x8E);
	idt_set_gate(0x0A, isr10, 0x08, 0x8E);
	idt_set_gate(0x0B, isr11, 0x08, 0x8E);
	idt_set_gate(0x0C, isr12, 0x08, 0x8E);
	idt_set_gate(0x0D, isr13, 0x08, 0x8E);
	idt_set_gate(0x0E, isr14, 0x08, 0x8E);
	idt_set_gate(0x0F, isr15, 0x08, 0x8E);
	idt_set_gate(0x10, isr16, 0x08, 0x8E);
	idt_set_gate(0x11, isr17, 0x08, 0x8E);
	idt_set_gate(0x12, isr18, 0x08, 0x8E);
	idt_set_gate(0x13, isr19, 0x08, 0x8E);
	idt_set_gate(0x14, isr20, 0x08, 0x8E);
	idt_set_gate(0x15, isr21, 0x08, 0x8E);
	idt_set_gate(0x16, isr22, 0x08, 0x8E);
	idt_set_gate(0x17, isr23, 0x08, 0x8E);
	idt_set_gate(0x18, isr24, 0x08, 0x8E);
	idt_set_gate(0x19, isr25, 0x08, 0x8E);
	idt_set_gate(0x1A, isr26, 0x08, 0x8E);
	idt_set_gate(0x1B, isr27, 0x08, 0x8E);
	idt_set_gate(0x1C, isr28, 0x08, 0x8E);
	idt_set_gate(0x1D, isr29, 0x08, 0x8E);
	idt_set_gate(0x1E, isr30, 0x08, 0x8E);
	idt_set_gate(0x1F, isr31, 0x08, 0x8E);

	struct idt_ptr_struct idt_ptr;
	idt_ptr.limit = (sizeof (struct idt_entry) * IDT_SIZE) - 1;
	idt_ptr.base = (intptr_t)&idt;
	idt_load(&idt_ptr);
}

