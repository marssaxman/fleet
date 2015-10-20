#include <string.h>

int strcmp(const char *l, const char *r)
{
	while (*l && *l == *r) {
		l++;
		r++;
	}
	return *(const unsigned char *)l - *(const unsigned char *)r;
}

