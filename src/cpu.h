// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef CPU_H
#define CPU_H

// Write some data to an IO port or read some data in.
extern void _outb(unsigned port, unsigned val);
extern unsigned _inb(unsigned port);
extern void _outl(unsigned port, unsigned val);
extern unsigned _inl(unsigned port);

#endif // CPU_H
