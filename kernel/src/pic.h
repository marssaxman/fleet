// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void _pic_init();
void _pic_eoi();
void _pic_mask(uint16_t);

#endif //PIC_H

