#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

// Errors are signified by returning the relevant error code * -1; positive
// or zero result indicates success.

// Success = 0.
int close(int streamid);
int flush(int streamid);

// Success = bytes read/written.
int write(int streamid, const void *src, unsigned bytes);
int read(int streamid, void *dest, unsigned bytes);

#endif //_SYS_SYSCALL_H
