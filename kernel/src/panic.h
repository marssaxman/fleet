#ifndef PANIC_H
#define PANIC_H

void _panic(const char *msg, ...);
void _kassertfail(const char *file, int line, const char *cond);
#define assert(c) ((c)? ((void)0): _kassertfail(__FILE__, __LINE__, #c))

#endif // PANIC_H
