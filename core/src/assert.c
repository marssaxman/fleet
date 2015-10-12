#include <assert.h>
#include "panic.h"

void _assert(const char *file, const char *line, const char *cond)
{
	_panic("assertion failed at %s:%s: %s\n", file, line, cond);
}

