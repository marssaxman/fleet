#ifndef _LIBC_SYSTEM_H
#define _LIBC_SYSTEM_H

// Startup initialization interface

#include "stream.h"

// The runtime host must populate these stream objects before invoking main.
extern _libc_stream _stdin_stream;
extern _libc_stream _stdout_stream;
extern _libc_stream _stderr_stream;

// The runtime should call _libc_main() when the environment is ready. It
// will set up the argument vector and invoke main().
void _libc_main(const char *cmdline);

extern int main(int argc, char **argv);

#endif //_LIBC_SYSTEM_H

