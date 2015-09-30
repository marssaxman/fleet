#ifndef _I386_CPU_H
#define _I386_CPU_H

// Disable or enable hardware interrupts.
extern void _cli();
extern void _sti();

// Fire an interrupt.
extern void _int3();

// Halt the cpu until the next hardware interrupt.
extern void _hlt();

#endif //_I386_CPU_H
