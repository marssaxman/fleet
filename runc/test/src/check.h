#ifndef CHECK_H
#define CHECK_H

void pass(const char *func);
void fail(const char *func, int line, const char *cond);
#define CHECK(cond) \
		(cond)? pass(__func__): fail(__func__, __LINE__, #cond)

void check_mem(
		const void *actual,
		const void *expect,
		int bytes,
		const char *func,
		int line);
#define CHECK_MEM(actual, expect, bytes) \
		check_mem((actual), (expect), (bytes), __func__, __LINE__)

void suite_exit();

#endif //CHECK_H
