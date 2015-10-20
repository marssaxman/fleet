#include "debug-write.h"
#include "test-string.h"
#include "check.h"

int main()
{
	debug_putc('\n');
	debug_write("BEGIN runc test suite\n");
	test_string();
	suite_exit();
	debug_write("END runc test suite\n");
	while (1);
}

