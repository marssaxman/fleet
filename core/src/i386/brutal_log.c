#include "i386/brutal_log.h"

static uint16_t *terminal_buffer = (uint16_t*) 0xB8000;
static uint16_t *last_line_buffer = (uint16_t*)0xB8780;
static uint16_t text_column = 0;
static uint16_t text_row = 0;
static void brutal_rollup()
{
	uint16_t *dest = &terminal_buffer[0];
	uint16_t *src = &terminal_buffer[80];
	for (unsigned i = 0; i < (80*24); ++i) {
		*dest++ = *src++;
	}
	for (unsigned i = 0; i < 80; ++i) {
		last_line_buffer[i] = 0x0F00 | ' ';
	}
}

void brutal_newline()
{
	brutal_rollup();
	text_column = 0;
}

void brutal_putch(char ch)
{
	last_line_buffer[text_column++] = 0x0F00 | ch;
	if (text_column >= 80) {
		brutal_newline();
	}
}

void brutal_puts(const char *msg)
{
	for (unsigned i = 0; i < 80 && *msg; ++i) {
		brutal_putch(*msg++);
	}
}

void brutal_putln(const char *msg)
{
	brutal_puts(msg);
	brutal_newline();
}

void brutal_showhex(const void *src, uint16_t digits)
{
	const uint8_t *buf = (const uint8_t*)src;
	const char table[] = "0123456789ABCDEF";
	while (digits--) {
		uint8_t byte = *buf++;
		brutal_putch(table[(byte >> 4) & 0x0F]);
		brutal_putch(table[byte & 0x0F]);
	}
}
