#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

struct _gdt_entry
{
	uint16_t limit;
	uint16_t base0_15;
	uint8_t base16_23;
	uint8_t attrs0_7;
	uint8_t attrs8_15;
	uint8_t base24_31;
} __attribute__ ((packed));

struct _gdt_register
{
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));

extern struct _gdt_register _gdtr;
extern struct _gdt_entry _gdt[5];

#endif //_GDT_H

