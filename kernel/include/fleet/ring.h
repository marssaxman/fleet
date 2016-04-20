// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _FLEET_RING_H
#define _FLEET_RING_H

#include <stdint.h>
#include <stddef.h>

// A ring_list is the master link joining the head and tail of a circular,
// doubly-linked list of structs each containing a ring_item, forming a deque.
struct ring_list { void *_[2]; };
struct ring_item { void *_[2]; };
void ring_init(struct ring_list*);

// Put an item onto the front or push it onto the back.
void ring_put(struct ring_list*, struct ring_item*);
void ring_push(struct ring_list*, struct ring_item*);

// Pull an item from the front or pop it from the back.
struct ring_item* ring_pull(struct ring_list*);
struct ring_item* ring_pop(struct ring_list*);

#define container_of(ptr, type, field) \
		((type*)(((intptr_t)ptr) - offsetof(type, field)))

#endif //_FLEET_RING_H

