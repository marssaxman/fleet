#ifndef _LIBC_SYSCALLS_H
#define _LIBC_SYSCALLS_H

extern void _exit(int status);
extern long _clock();
extern long _time();
extern void *_page_alloc(size_t n);
extern void *_page_free(void *base, size_t n);

#endif //_LIBC_SYSCALLS_H

