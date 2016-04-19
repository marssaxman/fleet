// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _FLEET_STREAM_H
#define _FLEET_STREAM_H

#include <fleet/system.h>

typedef unsigned stream_socket;

struct stream_transfer {
	struct ring_item link;
	struct {
		void *buffer;
		size_t size;
	} request;
	struct {
		unsigned error;
		size_t length;
	} response;
	struct task event;
};

unsigned transmit(stream_socket, struct stream_transfer*);
unsigned receive(stream_socket, struct stream_transfer*);

#endif //_FLEET_STREAM_H

