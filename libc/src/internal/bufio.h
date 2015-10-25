#ifndef _INTERNAL_STDIO_H
#define _INTERNAL_STDIO_H

#include <stdio.h>

// Return 0 on success, < 0 on failure.
int _close(FILE*);
int _sync(FILE*);

// Return size read/written on success, 0 on failure.
size_t _read(FILE*, void*, size_t);
size_t _write(FILE*, const void*, size_t);

void _stdio_init();

#endif //_INTERNAL_STDIO_H

