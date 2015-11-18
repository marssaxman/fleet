#include <sys/entry.h>
#include <sys/stdio.h>
#include <stdio.h>
#include "internal/stream.h"

extern int main(int argc, char *argv[]);

struct _stream _stdin;
FILE *stdin = &_stdin;
struct _stream _stdout;
FILE *stdout = &_stdout;
struct _stream _stderr;
FILE *stderr = &_stderr;

int _main(const char *cmdline)
{
	_init_stream(&_stdin);
	_init_stream(&_stdout);
	_init_stream(&_stderr);
	_stdin.id = _stdin_id;
	_stdout.id = _stdout_id;
	_stderr.id = _stderr_id;
	return main(0, 0);
}

