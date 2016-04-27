// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef CPU_H
#define CPU_H

static inline void _cpu_int_enable() {
	__asm__("sti");
}

static inline void _cpu_int_disable() {
	__asm__("cli");
}

static inline void _cpu_halt() {
	__asm__("hlt");
}

void _cpu_reset();

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0,%1" : : "a" (val), "dN" (port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ __volatile__("outw %0,%1" : : "a" (val), "dN" (port));
}

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ __volatile__("outl %0,%1" : : "a" (val), "dN" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ __volatile__("inb %1,%0" : "=a" (val) : "dN" (port));
    return val;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t val;
    __asm__ __volatile__("inw %1,%0" : "=a" (val) : "dN" (port));
    return val;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t val;
    __asm__ __volatile__("inl %1,%0" : "=a" (val) : "dN" (port));
    return val;
}

#endif //CPU_H

