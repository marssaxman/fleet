// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <fleet/io.h>
#include "serial.h"

// add check: ensure that transfer link is null, not currently attached

unsigned transmit(int socket, struct stream_transfer *t) {
	t->response.error = 0;
	t->response.length = 0;
	return _serial_transmit(socket, t);
}

unsigned receive(int socket, struct stream_transfer *t) {
	t->response.error = 0;
	t->response.length = 0;
	return _serial_receive(socket, t);
}


