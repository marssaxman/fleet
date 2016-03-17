#include <inttypes.h>
#include <stdlib.h>

intmax_t strtoimax(const char *restrict s, char **restrict p, int base)
{
    return strtoll(s, p, base);
}

