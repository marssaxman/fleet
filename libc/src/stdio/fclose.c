#include "internal/stdio.h"
#include "internal/stream.h"
#include <sys/syscall.h>

int fclose(FILE *stream)
{
	int syncret = _sync(stream);
	int closeret = _close(stream);
	return (syncret || closeret)? EOF: 0;
}

