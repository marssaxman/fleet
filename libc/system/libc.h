#ifndef _LIBC_SYSTEM_H
#define _LIBC_SYSTEM_H

#include "stream.h"
#include "syscalls.h"
#include "config.h"

void _libc_init(const struct __libc_syscalls*, const struct _libc_config*);

#endif //_LIBC_SYSTEM_H

