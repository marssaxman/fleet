#ifndef _PANIC_H
#define _PANIC_H

void _panic(const char *msg, ...);
#define _assert(cond) \
	do { if (!(cond)) _panic("failed assertion! "#cond); } while (0)

#endif //_PANIC_H
