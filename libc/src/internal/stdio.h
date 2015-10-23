#ifndef _INTERNAL_STDIO_H
#define _INTERNAL_STDIO_H

#include <stdio.h>

size_t _read(FILE*, void*, size_t);
size_t _write(FILE*, const void*, size_t);

#endif //_INTERNAL_STDIO_H

