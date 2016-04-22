// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>

// Low-level UART control functions which are more conveniently implemented in
// assembly language, supporting the actual device driver in serial.c.

int _uart_probe(uint16_t addr);

void _uart_open(uint16_t addr);
void _uart_close(uint16_t addr);
void _uart_tx_start(uint16_t addr);
void _uart_tx_stop(uint16_t addr);
void _uart_rx_start(uint16_t addr);
void _uart_rx_stop(uint16_t addr);

uint8_t _uart_IIR(uint16_t addr);
uint8_t _uart_LSR(uint16_t addr);
uint8_t _uart_MSR(uint16_t addr);

struct uart_buf { void *base; size_t size; };
void _uart_tx_service(uint16_t addr, struct uart_buf*);
void _uart_rx_service(uint16_t addr, struct uart_buf*);

#endif //UART_H

