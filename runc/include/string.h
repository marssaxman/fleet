#ifndef _STRING_H
#define _STRING_H

#include <stddef.h> // for size_t

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

// Fill
void *memset(void *buf, int ch, size_t n); // in startc

// Copy
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
void *memmove(void *dest, const void *src, size_t n); // in startc
void *memcpy(void *dest, const void *src, size_t n); // in startc

// Compare
int memcmp(const void *l, const void *r, size_t n); // in startc
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
char *strtok_r(char *str, const char *delims, char **lasts);

// Misc
char *strerror(int num);
size_t strlen(const char *str);
size_t strxfrm(char *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif //_STRING_H

