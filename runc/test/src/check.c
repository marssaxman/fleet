#include "check.h"
#include "debug-write.h"

static const char *current_func = 0;
static unsigned current_fails = 0;

static void print_dec(int num)
{
	char buf[12];
	int i = 12;
	buf[--i] = '\0';
	while (num) {
		buf[--i] = (num % 10) + '0';
		num /= 10;
	}
	debug_write(&buf[i]);
}

static void write_test_conclusion()
{
	if (0 == current_func) return;
	// We have finished the previous function and have just passed the first
	// test in a new function. If all tests in the previous function passed,
	// print a message to that effect.
	debug_write(current_func);
	debug_write(": ");
	if (0 == current_fails) {
		debug_write("pass");
	} else {
		print_dec(current_fails);
		debug_write(" check");
		if (current_fails > 1) {
			debug_putc('s');
		}
		debug_write(" failed");
	}
	debug_putc('\n');
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

void pass(const char *func)
{
	check_test_conclusion(func);
}

void fail(const char *func, int line, const char *cond)
{
	check_test_conclusion(func);
	current_fails++;
	debug_write("FAIL ");
	debug_write(func);
	debug_write("@");
	print_dec(line);
	debug_write(": ");
	debug_write(cond);
	debug_putc('\n');
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
			debug_write("\texpect: ");
			debug_print_buffer(expect, bytes);
			debug_write("\n\tactual: ");
			debug_print_buffer(actual, bytes);
			debug_putc('\n');
			return;
		}
	}
	pass(func);
}

void suite_exit()
{
	write_test_conclusion();
}

