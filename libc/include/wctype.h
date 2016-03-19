// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <c/wint_t.h>
#include <c/weof.h>

typedef const int *wctrans_t;
typedef unsigned long wctype_t;

#undef WEOF
#define WEOF 0xffffffffU

int iswalnum(wint_t);
int iswalpha(wint_t);
int iswblank(wint_t);
int iswcntrl(wint_t);
int iswdigit(wint_t);
int iswgraph(wint_t);
int iswlower(wint_t);
int iswprint(wint_t);
int iswpunct(wint_t);
int iswspace(wint_t);
int iswupper(wint_t);
int iswxdigit(wint_t);
int iswctype(wint_t, wctype_t);
wctrans_t wctrans(const char*);
wctype_t wctype(const char*);
wint_t towctrans(wint_t c, wctrans_t desc);
wint_t towlower(wint_t wc);
wint_t towupper(wint_t wc);

#endif //_WCTYPE_H

