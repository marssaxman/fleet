#include <stdlib.h>

float strtof(const char *str, char **end)
{
	return (float)strtod(str, end);
}
