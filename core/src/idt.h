#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

struct _idt_entry
{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint16_t offset_high;
} __attribute__((packed));

extern struct _idt_entry _idt[256];

// Macros for flags value subfields
#define _IDT_GATE_PRESENT_MASK 0x80
#define _IDT_GATE_PRESENT_TRUE 0x80
#define _IDT_GATE_PRESENT_FALSE 0x00

#define _IDT_GATE_DPL_MASK 0x60
#define _IDT_GATE_DPL_SUPERVISOR 0x00
#define _IDT_GATE_DPL_USER 0x60

#define _IDT_GATE_SEGMENT_MASK 0x10
#define _IDT_GATE_SEGMENT_TRUE 0x10
#define _IDT_GATE_SEGMENT_FALSE 0x00

#define _IDT_GATE_TYPE_MASK 0x0F
#define _IDT_GATE_TYPE_TASK32 0x5
#define _IDT_GATE_TYPE_INTR16 0x6
#define _IDT_GATE_TYPE_TRAP16 0x7
#define _IDT_GATE_TYPE_INTR32 0xE
#define _IDT_GATE_TYPE_TRAP32 0xF

struct _idt_descriptor
{
	uint16_t size;
	uint32_t offset;
} __attribute__((packed));
extern struct _idt_descriptor _idtptr;

extern void _idt_init();
extern void _idt_load(struct _idt_descriptor*);

#endif //_IDT_H
