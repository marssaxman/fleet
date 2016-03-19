#include <wchar.h>
#include <stdio.h>

int wctob(wint_t wc)
{
	return (wc >= 0 && wc < 128)? wc: EOF;
}
