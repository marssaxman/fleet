#include "internal/bufio.h"

int puts(const char *str)
{
	int out = fputs(str, stdout);
	if (out < 0) return out;
	if (0 == putchar('\n')) return EOF;
	return out + 1;
}

