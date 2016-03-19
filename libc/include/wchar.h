// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _WCHAR_H
#define _WCHAR_H

#include <c/null.h>
#include <c/wchar_t.h>
#include <c/size_t.h>
#include <c/struct_tm.h>
#include <c/wint_t.h>
#include <c/weof.h>
#include <c/wchar_min_max.h>

// fgetwc
// fgetws
// fputwc
// fputws
// fwide
// fwprintf
// fwscanf
// getwc
// getwchar
// putwc
// putwchar
// swprintf
// swscanf
// ungetwc
// vfwprintf
// vwprintf
// wprintf
// wscanf

// wcstod
// wcstol
// wcstoul

typedef struct __mbstate { int reserved; } mbstate_t;

wint_t btowc(int c);
// mbrlen
// mbrtowc
int mbsinit(const mbstate_t *s);
// mbsrtowcs
// wcrtomb
int wctob(wint_t);
// wcsrtombs

wchar_t *wcscat(wchar_t *restrict dest, const wchar_t *restrict src);
wchar_t *wcschr(const wchar_t *s, wchar_t c);
int wcscmp(const wchar_t *l, const wchar_t *r);
int wcscoll(const wchar_t *l, const wchar_t *r);
wchar_t *wcscpy(wchar_t *restrict d, const wchar_t *restrict s);
size_t wcscspn(const wchar_t *s, const wchar_t *c);
size_t wcslen(const wchar_t *s);
wchar_t *wcsncat(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
int wcsncmp(const wchar_t *l, const wchar_t *r, size_t n);
wchar_t *wcsncpy(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
wchar_t *wcspbrk(const wchar_t *s, const wchar_t *b);
wchar_t *wcsrchr(const wchar_t *s, wchar_t c);
size_t wcsspn(const wchar_t *s, const wchar_t *c);
wchar_t *wcsstr(const wchar_t *restrict h, const wchar_t *restrict n);
wchar_t *wcstok(wchar_t *restrict s, const wchar_t *restrict sep, wchar_t **restrict p);
size_t wcsxfrm(wchar_t *restrict dest, const wchar_t *restrict src, size_t n);
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
int wmemcmp(const wchar_t *l, const wchar_t *r, size_t n);
wchar_t *wmemcpy(wchar_t *restrict d, const wchar_t *restrict s, size_t n);
wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n);
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n);

// wcsftime

#endif // _WCHAR_H

