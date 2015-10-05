#ifndef _STARTC_ENTRY_H
#define _STARTC_ENTRY_H

// The application should define these entrypoint functions.

// Program entry
struct multiboot_info;
extern void _startc(unsigned magic, struct multiboot_info *info);

// CPU exception interrupt
struct _cpu_state;
extern void _isr_cpu(unsigned code, struct _cpu_state*);

// External device requested interrupt
extern void _isr_irq(unsigned irq);

#endif //_STARTC_ENTRY_H
