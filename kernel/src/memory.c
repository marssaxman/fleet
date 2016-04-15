// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "memory.h"
#include "multiboot.h"
#include "debug.h"
#include <stdint.h>

// The multiboot standard doesn't define memory region type codes except to
// say that type 1 is RAM.
enum {
	MAP_RAM = 1,
};

#define _PAGESIZE 4096

// Keep track of what lives where in our address space and which parts of it
// might be useful if we needed to store things. For the time being we're going
// to do this in a very simple 'sbrk' style, so we're only interested in the
// largest contiguous block of RAM. 
struct memory_map _memory;

typedef struct multiboot_memory_map bootmap;

static void check_region(bootmap *region) {
	// We only care about non-reserved regions.
	if (region->type != MAP_RAM) return;
	// We are built for a 32-bit machine, so we can't use anything above 4G
	// should the machine happen to include it.
	if (region->base_addr_high != 0) return;
	uint32_t base_addr = region->base_addr_low;
	// Nor can we use blocks which are greater than 4G, or which begin below
	// 4G but extend past it.
	uint32_t region_length = region->length_low;
	// How does this region compare to the ones we have seen previously? If we
	// have already found something better, we'll stick with it.
	if (region_length < (_memory.end - _memory.base)) return;
	// This is the best option we've found yet.
	_memory.base = (void*)base_addr;
	_memory.end = (void*)(base_addr + region_length);
}

static void map_check(bootmap *mmap, uint32_t size) {
	// Iterate through the address space regions defined in the BIOS
	// memory map and see if any of them are suitable for our allocator.
	bootmap *mapend = (bootmap*)((char*)mmap + size);
	while (mmap < mapend) {
		check_region(mmap);
		uint32_t entry_size = mmap->size + sizeof(mmap->size);
		mmap = (bootmap*) ((uint32_t) mmap + entry_size);
	}
}

static void simple_check(uint32_t bytes_lower, uint32_t bytes_upper) {
	// There's no memory map, but we know the sizes and starting addresses
	// of the lower and upper memory regions.
	uint32_t sz = sizeof(bootmap) - sizeof(uint32_t);
	bootmap lower = {sz, 0x00000000, 0, bytes_lower, 0, MAP_RAM};
	check_region(&lower);
	bootmap upper = {sz, 0x00100000, 0, bytes_upper, 0, MAP_RAM};
	check_region(&upper);
}

void _memory_init(struct multiboot_info *info) {
	// No matter what we figure out about the memory map, right now we know
	// we have memory from the base of the executable to the end of the
	// bootstrap stack.
	void *load_image_base = (void*)0x00100000; // from linker script
	extern int __data_end; // from linker script
	_memory.base = load_image_base;
	_memory.end = &__data_end; // allocated in libstart
	_memory.pos = _memory.end;
	// Use whatever information the bootloader gave us to figure out what
	// lives where in our address space and which parts of it we can use.
	if (info->flags & 1<<6) {
		// We have a BIOS memory map.
		map_check((bootmap*)info->memory_map_addr, info->memory_map_length);
	} else if (info->flags & 1<<0) {
		// We know how large the upper and lower memory banks are.
		simple_check(info->mem_lower * 1024, info->mem_upper * 1024);
	}
	if (load_image_base < _memory.end && (void*)&__data_end > _memory.base) {
		// The beginning of our memory region is already home to our
		// executable image and bootstrap stack. Move the allocation pointer
		// past it so we don't accidentally reuse it.
		_memory.pos = &__data_end;
	} else {
		_memory.pos = _memory.base;
	}
	if (load_image_base > _memory.base && load_image_base < _memory.end) {
		// Truncate the memory region so it no longer overlaps the executable.
		_memory.end = load_image_base;
	}
}

