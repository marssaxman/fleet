// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "syscalls.h"
#include <stddef.h>
#include <string.h>

// naive bootstrap allocator

static void *initbrk = 0;
static void *curbrk = 0;
static int total_size = 0;

static void *moreheap(size_t bytes)
{
	curbrk = sbrk(bytes);
	if (!initbrk) initbrk = curbrk;
	return curbrk;
}

struct block {
	struct block *next;
	size_t size;
} *freelist;

static struct block *header(void *ptr)
{
	return (struct block*)((intptr_t)ptr - sizeof(struct block));
}

static void *body(struct block *b)
{
	return (void*)((intptr_t)b + sizeof(struct block));
}

static void *end(struct block *b)
{
	return (void*)((intptr_t)b + sizeof(struct block) + b->size);
}

void *malloc(size_t bytes)
{
	struct block **chain = &freelist;
	// try to find existing free space which can satisfy this allocation
	for (struct block *b = *chain; b; chain = &b->next, b = *chain) {
		if (b->size >= bytes) {
			*chain = b->next;
			b->next = 0;
			total_size += b->size;
			return body(b);
		}
	}
	// We didn't find any free block large enough, so ask the system for some
	// more heap space.
	void *out = moreheap(bytes + sizeof(struct block));
	if (!initbrk) initbrk = out;
	((struct block*)out)->next = 0;
	((struct block*)out)->size = bytes;
	total_size += bytes;
	return body(out);
}

void *calloc(size_t num, size_t size)
{
	if ((SIZE_MAX / size) < num) return NULL;
	size_t total = size * num;
	void *out = malloc(total);
	if (out) memset(out, 0, total);
	return out;
}

void free(void *ptr)
{
	struct block *b = header(ptr);
	b->next = freelist;
	total_size -= b->size;
	freelist = b;
}

void *realloc(void *ptr, size_t size)
{
	// If there was no block before, go create a new one now.
	if (!ptr) return malloc(size);
	// If the block this data lives in can accommodate the requested size,
	// leave the data in place.
	struct block *b = header(ptr);
	if (b->size >= size) return ptr;
	// Allocate new space at the requested size, copy the old data over, then
	// free the old space.
	void *out = malloc(size);
	if (out) {
		memcpy(out, ptr, b->size);
		free(ptr);
	}
	return out;
}

#ifdef TESTSUITE
TESTSUITE(format) {
	// allocate 100 blocks of varying sizes in ascending order, then free them,
	// then allocate them again: we should not allocate any additional memory
	// the second time around.
	char *bufs[100];
	int before_size = total_size;
	for (int i = 0; i < 100; ++i) bufs[i] = malloc(i + 1);
	CHECK(total_size > before_size);
	for (int i = 0; i < 100; ++i) free(bufs[i]);
	CHECK(total_size == before_size);

	// allocate & free in reverse order, verify that it still works
	for (int i = 0; i < 100; ++i) bufs[i] = malloc(i + 1);
	for (int i = 100; i > 0; --i) free(bufs[i-1]);
	CHECK(total_size == before_size);

	// allocate some buffers, populate them, then free half of them: verify
	// that none of the contents of the other half have changed.
	for (int i = 0; i < 100; ++i) {
		bufs[i] = malloc(i + 1);
		memset(bufs[i], i, i+1);
	}
	for (int i = 0; i < 100; i += 2) {
		free(bufs[i]);
		bufs[i] = 0;
	}
	for (int i = 0; i < 100; ++i) {
		if (bufs[i]) {
			for (unsigned j = 0; j + 1 < i; ++j) {
				CHECK(bufs[i][j] == i);
			}
			free(bufs[i]);
		}
	}
}
#endif
