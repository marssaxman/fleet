// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdio.h>
#include "stream.h"

int ungetc(int character, FILE *stream)
{
	if (character == EOF) {
		return EOF;
	}
	if (stream->state & (STREAM_WRITE|STREAM_UNGET)) {
		return EOF;
	}
	stream->state &= ~STREAM_EOF;
	stream->state |= (STREAM_READ|STREAM_UNGET);
	stream->unget = character;
	return character;
}

#ifdef TESTSUITE
#include "memsocket.h"
#include <string.h>
#include <sys/socket.h>
char *message = "Hello world, this is a test.";
static struct memsocket ms;
static struct _stream stream;
static void initstream(void)
{
	_init_stream(&stream);
	stream.id = open_memsocket(message, strlen(message), &ms);
	ms.data_len = ms.buf_size;
	CHECK(ms.data_len == strlen(message));
	CHECK(stream.id >= 0);
}
static void exitstream(void)
{
	close(stream.id);
	_exit_stream(&stream);
}
static void resetstream(void)
{
	exitstream();
	initstream();
}
TESTSUITE(ungetc) {
	initstream();
	// Try it with fgetc.
	CHECK(fgetc(&stream) == 'H');
	CHECK(ungetc('B', &stream) == 'B');
	CHECK(fgetc(&stream) == 'B');

	// Try it with fgets.
	resetstream();
	CHECK(fgetc(&stream) == 'H');
	CHECK(ungetc('H', &stream) == 'H');
	char buf[80];
	CHECK(fgets(buf, strlen(message)+1, &stream) != NULL);
	CHECK(0 == strcmp(buf, message));

	// Try it with fread.
	resetstream();
	CHECK(fgetc(&stream) == 'H');
	CHECK(ungetc('J', &stream) == 'J');
	CHECK(fread(buf, 11, 1, &stream) == 1);
	CHECK(0 == strncmp(buf, "Jello world", 11));
	exitstream();
}
#endif //TESTSUITE

