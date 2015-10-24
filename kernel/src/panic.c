#include "panic.h"
#include "log.h"

void _panic(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	_log_print("panic: ");
	_log_vprintf(msg, args);
	while (1) {}
}

void _kassertfail(const char *file, int line, const char *cond)
{
	_panic("assertion failed at %s:%d: %s\n", file, line, cond);
}

