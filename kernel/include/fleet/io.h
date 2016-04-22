// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _FLEET_IO_H
#define _FLEET_IO_H

#include <stddef.h>
#include <fleet/ring.h>
#include <fleet/event.h>

struct stream_transfer {
	struct {
		void *buffer;
		size_t length;
	} request;
	struct {
		unsigned error;
		size_t length;
	} response;
	struct event signal;
	struct ring_item queue;
};

unsigned transmit(int socket, struct stream_transfer*);
unsigned receive(int socket, struct stream_transfer*);

#endif //_FLEET_IO_H


