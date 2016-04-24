// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "memory.h"
#include "multiboot.h"
#include "debug.h"
#include "kstring.h"
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
static void *memory_base;
static void *memory_break;
static void *memory_end;

static void check_region(struct memory_map *region) {
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
	if (region_length < (memory_end - memory_base)) return;
	// This is the best option we've found yet.
	memory_base = (void*)base_addr;
	memory_end = (void*)(base_addr + region_length);
}

static void map_check(struct memory_map *mmap, uint32_t size) {
	// Iterate through the address space regions defined in the BIOS
	// memory map and see if any of them are suitable for our allocator.
	struct memory_map *mapend = (struct memory_map*)((char*)mmap + size);
	while (mmap < mapend) {
		check_region(mmap);
		uint32_t entry_size = mmap->size + sizeof(mmap->size);
		mmap = (struct memory_map*) ((uint32_t) mmap + entry_size);
	}
}

static void simple_check(uint32_t bytes_lower, uint32_t bytes_upper) {
	// There's no memory map, but we know the sizes and starting addresses
	// of the lower and upper memory regions.
	uint32_t sz = sizeof(struct memory_map) - sizeof(uint32_t);
	struct memory_map lower = {sz, 0x00000000, 0, bytes_lower, 0, MAP_RAM};
	check_region(&lower);
	struct memory_map upper = {sz, 0x00100000, 0, bytes_upper, 0, MAP_RAM};
	check_region(&upper);
}

void _memory_init(struct multiboot_info *info) {
	// No matter what we figure out about the memory map, right now we know
	// we have at least as much memory as it takes to contain the executable.
	void *load_image_base = (void*)0x00100000; // from linker script
	extern int __data_end; // from linker script
	memory_base = load_image_base;
	memory_end = &__data_end;
	memory_break = memory_end;
	// Use whatever information the bootloader gave us to figure out what
	// lives where in our address space and which parts of it we can use.
	if (info->flags & 1<<6) {
		// We have a BIOS memory map.
		struct memory_map *mmap = (struct memory_map*)info->memory_map_addr;
		map_check(mmap, info->memory_map_length);
	} else if (info->flags & 1<<0) {
		// We know how large the upper and lower memory banks are.
		simple_check(info->mem_lower * 1024, info->mem_upper * 1024);
	}
	if (load_image_base < memory_end && (void*)&__data_end > memory_base) {
		// The beginning of our memory region is already home to our
		// executable image and bootstrap stack. Move the allocation pointer
		// past it so we don't accidentally reuse it.
		memory_break = &__data_end;
	} else {
		memory_break = memory_base;
	}
	if (load_image_base > memory_base && load_image_base < memory_end) {
		// Truncate the memory region so it no longer overlaps the executable.
		memory_end = load_image_base;
	}
}

void *_kalloc(size_t bytes) {
	void *out = memory_break;
	// Round up to nearest word
	memory_break += ((bytes+3)&~3);
	_kassert(memory_break <= memory_end);
	return out;
}

void *_kalloc_zero(size_t bytes) {
	return _kmemzero(_kalloc(bytes), bytes);
}

