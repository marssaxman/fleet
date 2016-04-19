// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _FLEET_RING_H
#define _FLEET_RING_H

#include <stdint.h>

struct ring_list { void *_[2]; };
struct ring_item { void *_[2]; };

void ring_init(struct ring_list*);
void ring_push(struct ring_list*, struct ring_item*); // add to end
struct ring_item* ring_pop(struct ring_list*); // remove last
struct ring_item* ring_pull(struct ring_list*); // remove first

#define ring_item_struct(ptr, type, field) \
		((type*)(((intptr_t)ptr) - offsetof(type,field)))

#endif //_FLEET_RING_H

