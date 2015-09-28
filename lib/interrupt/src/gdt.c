#include "gdt.h"
#include <stdint.h>

struct gdt_entry
{
	uint32_t limit_low;
	uint32_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr_struct
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

#define GDT_SIZE 5
static struct gdt_entry gdt[GDT_SIZE];

extern void gdt_load(void*);

static void gdt_set_gate(
		uint32_t num,
		uint32_t base,
		uint32_t limit,
		uint8_t access,
		uint8_t gran)
{
	struct gdt_entry *gate = &gdt[num];
	gate->base_low = (base & 0xFFFF);
	gate->base_middle = (base >> 16) & 0xFF;
	gate->base_high = (base >> 24) & 0xFF;
	gate->limit_low = (limit & 0xFFFF);
	gate->granularity = (limit >> 16) & 0x0F;
	gate->granularity |= gran & 0xF0;
	gate->access = access;
}

void gdt_init()
{
	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	struct gdt_ptr_struct gdt_ptr;
	gdt_ptr.limit = (sizeof(struct gdt_entry) * GDT_SIZE) - 1;
	gdt_ptr.base  = (uint32_t)&gdt;
	gdt_load(&gdt_ptr);
}

