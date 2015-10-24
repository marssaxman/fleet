#include "internal/stdio.h"
#include "internal/stream.h"
#include <sys/syscall.h>

int fclose(FILE *stream)
{
	return close(stream->id);
}

