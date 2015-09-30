#ifndef _I386_IDT_H
#define _I386_IDT_H

#include <stdint.h>

struct _idt_entry
{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint8_t offset_high;
};

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

extern void _idt_load(uint8_t size, _idt_entry[] table);

#endif //_I386_IDT_H
