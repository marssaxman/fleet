#include "test-string.h"
#include "check.h"
#include "debug-write.h"
#include <string.h>
#include <stdbool.h>


void test_string()
{
	// test startc implementations first, since these functions will come in
	// handy when testing the other functions
	test_memcmp();
	test_memcpy();
	test_memmove();
	test_memset();
}

