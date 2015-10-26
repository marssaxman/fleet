#ifndef _INTERNAL_BUFIO_H
#define _INTERNAL_BUFIO_H

#include <stdio.h>

// Return 0 on success, < 0 on failure.
int _close(FILE*);
int _flush(FILE*);
int _buffer(FILE*, char *buf, size_t size);

// Return size read/written on success, 0 on failure.
size_t _read(FILE*, void*, size_t);
size_t _write(FILE*, const void*, size_t);

#endif //_INTERNAL_BUFIO_H

