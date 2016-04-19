// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <fleet/stream.h>
#include "serial.h"

// add check: ensure that transfer link is null, not currently attached

unsigned transmit(stream_socket s, struct stream_transfer *t) {
	t->response.error = 0;
	t->response.length = 0;
	return _serial_transmit(s, t);
}

unsigned receive(stream_socket s, struct stream_transfer *t) {
	t->response.error = 0;
	t->response.length = 0;
	return _serial_receive(s, t);
}


