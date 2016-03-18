// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef CONSOLE_H
#define CONSOLE_H

#include <c/size_t.h>

// Write-only access to the hypervisor/emulator debugging console
void _console_write(const char *str, size_t len);
void _console_putc(int character);

#endif //CONSOLE_H

