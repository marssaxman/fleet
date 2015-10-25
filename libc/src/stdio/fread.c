#include "internal/bufio.h"

size_t fread(void *buf, size_t size, size_t count, FILE *stream)
{
	return _read(stream, buf, size*count) / size;
}


