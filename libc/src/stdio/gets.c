#include <stdio.h>
#include <limits.h>
#include <string.h>

char *gets(char *s)
{
	char *ret = fgets(s, INT_MAX, stdin);
	size_t len = ret? strlen(s): 0;
	if (len && s[len-1] == '\n') {
		s[len-1] = '\0';
	}
	return ret;
}
