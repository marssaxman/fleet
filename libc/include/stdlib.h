#ifndef _STDLIB_H
#define _STDLIB_H

#include <_null.h>
#include <_size_t.h>
#include <_wchar_t.h>

double atof(const char *str);
int atoi(const char *str);
long atol(const char *str);
long long atoll(const char *str);
float strtof(const char *str, char **end);
double strtod(const char *str, char **end);
long double strtold(const char *str, char **end);
long strtol(const char *str, char **end, int radix);
long long strtoll(const char *str, char **end, int radix);
unsigned long strtoul(const char *str, char **end, int radix);
unsigned long long strtoull(const char *str, char **end, int radix);

typedef int(*_compar)(const void*, const void*);
void *bsearch (const void *key, const void *base, size_t n, size_t w, _compar);
void qsort(void *base, size_t n, size_t w, _compar);

#if 0 // still to implement
EXIT_FAILURE
EXIT_SUCCESS
MB_CUR_MAX
RAND_MAX
_Exit
abort
abs
atexit
calloc
div
div_t
exit
free
getenv
labs
ldiv
ldiv_t
llabs
lldiv
lldiv_t
malloc
mblen
mbstowcs
mbtowc
rand
realloc
srand
system
wcstombs
wctomb
#endif // still to implement

#endif //_STDLIB_H

