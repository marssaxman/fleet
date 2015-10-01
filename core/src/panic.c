#include "panic.h"
#include <sys/log.h>

void panic(const char *msg)
{
	_log_printf("panic: %s\n", msg);
	while (1) {}
}
