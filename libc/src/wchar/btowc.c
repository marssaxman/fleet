#include <wchar.h>

wint_t btowc(int c)
{
	return (c >= 0 && c < 128)? c: WEOF;
}
