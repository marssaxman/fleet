#include <stdlib.h>

long atol(const char *str)
{
	return strtol(str, (char**)NULL, 10);
}
