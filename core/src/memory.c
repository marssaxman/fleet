#include "memory.h"
#include "multiboot.h"
#include "log.h"
#include "panic.h"

// The multiboot standard doesn't define memory region type codes except to
// say that type 1 is RAM.
enum {
	MAP_RAM = 1,
};

// Keep track of what lives where in our address space and which parts of it
// might be useful if we needed to store things.

static void check_region(struct memory_map *block)
{
	_log(MEMORY, "    0x%x%x  0x%x%x  0x%x\n",
			block->base_addr_high, block->base_addr_low,
			block->length_high, block->length_low,
			block->type);
}

static void map_init(struct memory_map *mmap, unsigned long size)
{
	struct memory_map *mapend = (struct memory_map*)((char*)mmap + size);
	while (mmap < mapend) {
		check_region(mmap);
		unsigned long entry_size = mmap->size + sizeof(mmap->size);
		mmap = (struct memory_map*) ((unsigned long) mmap + entry_size);
	}
}

static void simple_init(unsigned long bytes_lower, unsigned long bytes_upper)
{
	unsigned long mapsize = sizeof(struct memory_map) - sizeof(unsigned long);
	struct memory_map lower = {mapsize, 0, 0, bytes_lower, 0, MAP_RAM};
	check_region(&lower);
	struct memory_map upper = {mapsize, 0, 0, bytes_upper, 0, MAP_RAM};
	check_region(&upper);
}

void _memory_init(struct multiboot_info *info)
{
	_log(MEMORY, "Physical memory map\n");
	_log(MEMORY, "    Base                Size                Type\n");
	// How much information did the bootloader give us about this machine?
	if (info->flags & 1<<6) {
		// We have a BIOS memory map.
		struct memory_map *mmap = (struct memory_map*)info->mmap_addr;
		map_init(mmap, info->mmap_length);
	} else if (info->flags & 1<<0) {
		// No memory map, so we'll have to make do with bank sizes
		simple_init(info->mem_lower * 1024, info->mem_upper * 1024);
	} else {
		// No information at all!? This won't work.
		_panic("Bootloader failed to provide memory map\n");
	}
}

