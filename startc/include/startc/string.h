#ifndef _STARTC_STRING_H
#define _STARTC_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void *memcpy(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len);
void *memset(void*, int, size_t);

#ifdef __cplusplus
}
#endif

#endif // _STARTC_STRING_H
