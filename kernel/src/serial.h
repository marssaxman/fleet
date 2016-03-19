// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

// Primitive, low-speed legacy serial interfaces, aka COM ports.

struct serial;

extern struct serial COM1;
extern struct serial COM2;
extern struct serial COM3;
extern struct serial COM4;

int _serial_open(struct serial*);

#endif // SERIAL_H
