#ifndef _STARTC_STARTC_H
#define _STARTC_STARTC_H

// The application should define these entrypoint functions.

// Program entry
struct multiboot_info;
extern void _startc(unsigned magic, struct multiboot_info *info);

// CPU exception interrupt
struct _cpu_state;
extern void _isr_cpu(unsigned code, struct _cpu_state*);

#endif //_STARTC_STARTC_H
