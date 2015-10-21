#include "debug-write.h"
#include "check.h"

int main()
{
	suite_exit();
	debug_write("END runc test suite\n");
	return 0;
}

