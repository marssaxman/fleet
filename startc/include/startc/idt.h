#ifndef _STARTC_IDT_H
#define _STARTC_IDT_H

#include <stdint.h>

struct _idt_entry
{
	uint16_t offset0_15;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint16_t offset16_31;
} __attribute__((packed));

struct _idt_register
{
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));

extern struct _idt_register _idtr;
extern struct _idt_entry _idt[256];

#endif //_STARTC_IDT_H
