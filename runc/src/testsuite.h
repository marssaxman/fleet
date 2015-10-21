#ifndef __TESTSUITE_H
#define __TESTSUITE_H

#ifdef TEST_SUITE_MODE
#define TESTSUITE __attribute__((constructor)) static void test()

void check(int expect, const char *cond, const char *func, int line);
#define CHECK(cond) \
		check((cond), #cond, __FILE__, __LINE__)

void check_mem(
		const void *actual,
		const void *expect,
		int bytes,
		const char *func,
		int line);
#define CHECK_MEM(actual, expect, bytes) \
		check_mem((actual), (expect), (bytes), __FILE__, __LINE__)

void check_str(
		const char *actual,
		const char *expect,
		int max,
		const char *func,
		int line);
#define CHECK_STR(actual, expect, max) \
		check_str((actual), (expect), (max), __FILE__, __LINE__)

#else	// not TEST_SUITE_MODE
#define TESTSUITE __attribute__((unused)) static void test()

#define CHECK(x) while(0)
#define CHECK_MEM(a, e, b) while(0)
#define CHECK_STR(a, e, b) while(0)
#define TESTATTR unused

#endif

#endif //__TESTSUITE_H

