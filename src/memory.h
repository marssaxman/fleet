#ifndef _MEMORY_H
#define _MEMORY_H

struct multiboot_info;
void _memory_init(struct multiboot_info *info);

#define _PAGESIZE 4096
// Allocate a block of pages and return its base address.
// Always succeeds; panics when it runs out of memory.
void *_memory_alloc(unsigned page_count);

#endif //_MEMORY_H
