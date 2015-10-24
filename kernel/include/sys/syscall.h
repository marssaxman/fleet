#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int write(int streamid, const void *src, unsigned bytes);
int read(int streamid, void *dest, unsigned bytes);

#endif //_SYS_SYSCALL_H
