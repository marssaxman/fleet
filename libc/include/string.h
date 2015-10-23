#ifndef _STRING_H
#define _STRING_H

#include <_size_t.h>
#include <_null.h>

#ifdef __cplusplus
extern "C" {
#endif

// Fill
void *memset(void *buf, int ch, size_t n);

// Copy
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

// Compare
int memcmp(const void *l, const void *r, size_t n);
int strcmp(const char *l, const char *r);
int strncmp(const char *l, const char *r, size_t n);
int strcoll(const char *l, const char *r);

// Concatenate
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);

// Search
char *strchr(const char *str, int ch);
char *strrchr(const char *str, int ch);
char *strstr(const char *haystack, const char *needle);
void *memchr(const void *buf, int ch, size_t n);

// Tokenize
size_t strspn(const char *str, const char *set);
size_t strcspn(const char *str, const char *set);
char *strpbrk(const char *str, const char *set);
char *strtok(char *str, const char *delims);

// Misc
char *strerror(int num);
size_t strlen(const char *str);
size_t strxfrm(char *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif //_STRING_H

