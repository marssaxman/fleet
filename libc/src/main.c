#include <sys/entry.h>
#include "internal/stdio.h"

extern int main(int argc, char *argv[]);

int _main(const char *cmdline)
{
	_stdio_init();
	return main(0, 0);
}

