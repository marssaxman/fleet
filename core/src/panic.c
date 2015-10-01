#include "panic.h"
#include "log.h"

void _panic(const char *msg)
{
	_log_printf("panic: %s\n", msg);
	while (1) {}
}
