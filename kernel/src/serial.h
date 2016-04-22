// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef SERIAL_H
#define SERIAL_H

#include <fleet/io.h>

void _serial_init();
unsigned _serial_transmit(int socket, struct stream_transfer *t);
unsigned _serial_receive(int socket, struct stream_transfer *t);

#endif //SERIAL_H
