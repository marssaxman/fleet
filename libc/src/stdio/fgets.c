#include <stdio.h>

char *fgets(char *str, int num, FILE *stream)
{
	char *buf = str;
	// sanity check: make sure there is room in the buffer for at least one
	// character.
	if (num <= 1) {
		return NULL;
	}
	int ch = '\0';
	while (--num > 0) {
		ch = fgetc(stream);
		if (ch == EOF) {
			return NULL;
		}
		*str++ = ch;
		if (ch == '\n') {
			break;
		}
	}
	*str = '\0';
	return buf;
}


