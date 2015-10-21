#ifndef __TESTMODE_H
#define __TESTMODE_H

#ifdef TEST_SUITE_MODE
#define TESTBEGIN __attribute__((constructor)) static void test() {
#define TESTEND }
#else
#define TESTBEGIN #if 0
#define TESTEND #endif
#endif

#endif //__TESTMODE_H

