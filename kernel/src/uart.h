// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <fleet/system.h>

void _uart_init();

void _uart_transmit(unsigned port, void *buffer, unsigned size, struct task*);
void _uart_receive(unsigned port, void *buffer, unsigned size, struct task*);

extern void _uart_line_status(unsigned port, unsigned LSR);
extern void _uart_modem_status(unsigned port, unsigned MSR);

#endif //UART_H

