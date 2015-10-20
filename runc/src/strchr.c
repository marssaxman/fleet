#include <string.h>

char *strchr(const char *str, int ch)
{
	while (*str && *str != ch) {
		str++;
	}
	return (*str == ch) ? (char*)str : NULL;
}

