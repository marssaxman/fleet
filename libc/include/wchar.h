// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _WCHAR_H
#define _WCHAR_H

#include <_null.h>
#include <_wchar_t.h>
#include <_size_t.h>
#include <_struct_tm.h>
#include <_wint_t.h>
#include <_weof.h>
#include <_wchar_min_max.h>

size_t wcslen(const wchar_t *s);
wchar_t *wcschr(const wchar_t *s, wchar_t c);
wchar_t *wcscat(wchar_t *restrict dest, const wchar_t *restrict src);
int wcscmp(const wchar_t *l, const wchar_t *r);
wchar_t *wcscpy(wchar_t *restrict d, const wchar_t *restrict s);
size_t wcscspn(const wchar_t *s, const wchar_t *c);
wchar_t *wcsncat(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
int wcsncmp(const wchar_t *l, const wchar_t *r, size_t n);
wchar_t *wcsncpy(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
wchar_t *wcspbrk(const wchar_t *s, const wchar_t *b);
wchar_t *wcsrchr(const wchar_t *s, wchar_t c);
size_t wcsspn(const wchar_t *s, const wchar_t *c);
wchar_t *wcsstr(const wchar_t *restrict h, const wchar_t *restrict n);
wchar_t *wcstok(wchar_t *restrict s, const wchar_t *restrict sep, wchar_t **restrict p);
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n);
int wmemcmp(const wchar_t *l, const wchar_t *r, size_t n);
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
wchar_t *wmemcpy(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n);

#if 0 // still to implement
btowc
fgetwc
fgetws
fputwc
fputws
fwide
fwprintf
fwscanf
getwc
getwchar
mbrlen
mbrtowc
mbsinit
mbsrtowcs
mbstate_t
putwc
putwchar
swprintf
swscanf
ungetwc
vfwprintf
vfwscanf
vswprintf
vswscanf
vwprintf
vwscanf
wcrtomb
wcscoll
wcsftime
wcsrtombs
wcstod
wcstof
wcstol
wcstold
wcstoll
wcstoul
wcstoull
wcsxfrm
wctob
wprintf
wscanf
#endif // not yet implemented

#endif // _WCHAR_H

