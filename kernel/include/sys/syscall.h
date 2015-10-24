#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

int write(int streamid, const void *src, unsigned bytes);
int read(int streamid, void *dest, unsigned bytes);
int close(int streamid);

#endif //_SYS_SYSCALL_H
