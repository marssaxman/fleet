#include <string.h>

int strncmp(const char *l, const char *r, size_t n)
{
	while (n--) {
		unsigned char lch = *(unsigned char*)l++;
		unsigned char rch = *(unsigned char*)r++;
		if (lch != rch) return lch - rch;
		if (!lch) break;
	}
	return 0;
}

