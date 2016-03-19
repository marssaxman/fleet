// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include <stdarg.h>

int sscanf(const char *buffer, const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	int ret = vsscanf(buffer, format, arg);
	va_end(arg);
	return ret;
}

#ifdef TESTSUITE
#include <string.h>
#include <errno.h>
#include <limits.h>

#define TEST_F(x) ( \
CHECK(1 == sscanf(# x, "%lf", &d)), \
CHECK(x == d))

TESTSUITE(sscanf) {
	int i;
	int err=0;
	char a[100], b[100], *s;
	int x, y, z, u, v;
	double d, t;
	long lo[10];

	CHECK(2 == sscanf("hello, world\n", "%s %s", a, b));
	CHECK(0 == strcmp(a, "hello,"));
	CHECK(0 == strcmp(b, "world"));

	CHECK(2 == sscanf("hello, world\n", "%[hel]%s", a, b));
	CHECK(0 == strcmp(a, "hell"));
	CHECK(0 == strcmp(b, "o,"));

	CHECK(2 == sscanf("hello, world\n", "%[hel] %s", a, b));
	CHECK(0 == strcmp(a, "hell"));
	CHECK(0 == strcmp(b, "o,"));

	a[8] = 'X';
	a[9] = 0;
	CHECK(1 == sscanf("hello, world\n", "%8c%8c", a, b));
	CHECK(0 == strcmp(a, "hello, wX"));

	CHECK(1 == sscanf("56a72", "%*d %[0123456789]\n", a));
	CHECK(2 == sscanf("0123 56a72", "%d%*d %[0123456789]\n", &y, a));
	CHECK(3 == sscanf("56789 0123 56a72", "%2d%d%*d %[0123456789]\n", &x, &y, a));

	CHECK(x == 56);
	CHECK(y == 789);
	CHECK(0 == strcmp(a, "56"));

	CHECK(5 == sscanf("011 0x100 11 0x100 100", "%i %i %o %x %x\n", &x, &y, &z, &u, &v));
	CHECK(x == 9);
	CHECK(y == 256);
	CHECK(z == 9);
	CHECK(u == 256);
	CHECK(v == 256);

	CHECK(1 == sscanf("20 xyz", "%d %d\n", &x, &y));
	CHECK(x == 20);

	CHECK(0 == sscanf("xyz", "%d\n", &x, &y));

	CHECK(-1 == sscanf("", "%d\n", &x, &y));

	CHECK(3 == sscanf(" 12345 6", "%2d%d%d", &x, &y, &z));
	CHECK(x == 12);
	CHECK(y == 345);
	CHECK(z == 6);

	CHECK(1 == sscanf(" 0x12 0x34", "%5i%2i", &x, &y));
	CHECK(x == 0x12);

	TEST_F(123);
	TEST_F(123.0);
	TEST_F(123.0e+0);
	TEST_F(123.0e+4);
//	TEST_F(1.234e1234);
//	TEST_F(1.234e-1234);
//	TEST_F(1.234e56789);
//	TEST_F(1.234e-56789);
	TEST_F(-0.5);
	TEST_F(0.1);
	TEST_F(0.2);
	TEST_F(0.1e-10);
	TEST_F(0x1234p56);

	CHECK(0 == sscanf("10e", "%lf", &d));
	CHECK(-1 == sscanf("", "%lf\n", &d));
}
#endif
