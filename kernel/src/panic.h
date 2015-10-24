#ifndef _PANIC_H
#define _PANIC_H

void _panic(const char *msg, ...);
void _kassertfail(const char *file, int line, const char *cond);
#define assert(c) ((void)(c) || _kassertfail(__FILE__, __LINE__, #c))

#endif //_PANIC_H
