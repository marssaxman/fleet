#ifndef _CPU_H
#define _CPU_H

// Disable or enable hardware interrupts.
extern void _cli();
extern void _sti();

// Fire an interrupt.
extern void _int3();

// Halt the cpu until the next hardware interrupt.
extern void _hlt();

// Write some data to an IO port or read some data in.
extern void _outb(unsigned port, unsigned val);
extern unsigned _inb(unsigned port);
extern void _outl(unsigned port, unsigned val);
extern unsigned _inl(unsigned port);

#endif //_I386_CPU_H
