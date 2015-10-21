#include <stdio.h>

static const char *current_func = 0;
static unsigned current_fails = 0;

static void putstr(const char *str) { fputs(str, stdout); }

static void print_buffer(const void *buf, int bytes)
{
	static const char digits[] = "0123456789ABCDEF";
	const char *src = (const char*)buf;
	while (bytes--) {
		char ch = *src++;
		if (ch > 0x1F && ch < 0x7F) {
			putchar(ch);
		} else {
			putchar('\\');
			putchar('x');
			putchar(digits[ch & 0x0F]);
			putchar(digits[(ch >> 4) & 0x0F]);
		}
	}
}

static void print_dec(int num)
{
	char buf[12];
	int i = 12;
	buf[--i] = '\0';
	while (num) {
		buf[--i] = (num % 10) + '0';
		num /= 10;
	}
	putstr(&buf[i]);
}

static void write_func(const char *func)
{
	// we're actually getting file names instead of function names now, so
	// trim off the directory path and type extension if they are present.
	const char *lastslash = 0;
	for (const char *src = func; *src; ++src) {
		if (*src == '/') lastslash = src;
	}
	if (*++lastslash) {
		func = lastslash;
	}
	putstr(func);
}

static void write_test_conclusion()
{
	if (0 == current_func) return;
	// We have finished the previous function and have just passed the first
	// test in a new function. If all tests in the previous function passed,
	// print a message to that effect.
	write_func(current_func);
	putstr(": ");
	if (0 == current_fails) {
		putstr("pass");
	} else {
		print_dec(current_fails);
		putstr(" check");
		if (current_fails > 1) {
			putchar('s');
		}
		putstr(" failed");
	}
	putchar('\n');
}

static void check_test_conclusion(const char *func)
{
	// did the check we just made come from the same function as the previous
	// one? if so, the test has not ended. if we are in a new function, then
	// the previous one ended and we should print its status info.
	if (current_func == func) return;
	write_test_conclusion();
	current_fails = 0;
	current_func = func;
}

static void pass(const char *func)
{
	check_test_conclusion(func);
}

static void fail(const char *func, int line, const char *cond)
{
	check_test_conclusion(func);
	current_fails++;
	putstr("FAIL ");
	write_func(func);
	putstr("@");
	print_dec(line);
	putstr(": ");
	putstr(cond);
	putchar('\n');
}

void check(int flag, const char *cond, const char *func, int line)
{
	if (flag) pass(func);
	else fail(func, line, cond);
}

void check_mem(
		const void *actual,
		const void *expect,
		int bytes,
		const char *func,
		int line)
{
	for (int i = 0; i < bytes; ++i) {
		if (((unsigned char*)actual)[i] != ((unsigned char*)expect)[i]) {
			fail(func, line, "");
			putstr("\texpect: ");
			print_buffer(expect, bytes);
			putstr("\n\tactual: ");
			print_buffer(actual, bytes);
			putchar('\n');
			return;
		}
	}
	pass(func);
}

void check_str(
		const char *actual,
		const char *expect,
		int max,
		const char *func,
		int line)
{
	const char *aptr = actual;
	const char *eptr = expect;
	for (int i = 0; i < max; ++i) {
		if (*aptr != *eptr++) {
			fail(func, line, "");
			putstr("\texpect: ");
			putstr(expect);
			putstr("\n\tactual: ");
			putstr(actual);
			putchar('\n');
			return;
		}
		if (!*aptr++) break;
	}
	pass(func);
}

int main()
{
	write_test_conclusion();
	return 0;
}

