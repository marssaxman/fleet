#ifndef _LIBC_SYSCALLS_H
#define _LIBC_SYSCALLS_H

struct _libc_syscalls
{
	void (*exit)(int status);
	long (*clock)();
	long (*time)();
	void *(*page_alloc)(size_t n);
	void *(*page_free)(void *base, size_t n);
};

#endif //_LIBC_SYSCALLS_H

