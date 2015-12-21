#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

struct multiboot_info;
void _memory_init(struct multiboot_info *info);
void *_memory_alloc(size_t bytes);

#endif // MEMORY_H
