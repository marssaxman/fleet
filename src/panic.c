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
