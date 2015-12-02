#include <stdlib.h>

long long atoll(const char *str)
{
	return strtoll(str, (char**)NULL, 10);
}
