// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"
#include "socket.h"
#include <errno.h>
#include <string.h>

static int bufread(FILE *stream, char *dest, size_t bytes)
{
	size_t total = 0;
	char *buf_end = stream->buf_addr + stream->buf_size;
	if (stream->buf_count > 0) {
		// There is data in this input buffer. Copy as much of it into the
		// destination buffer as we can fit.
		size_t space = bytes - total;
		size_t copy = (space > stream->buf_count)? stream->buf_count: space;
		memcpy(dest, buf_end - stream->buf_count, copy);
		stream->buf_count -= copy;
		total += copy;
		dest += copy;
	}
	size_t remaining = bytes - total;
	if (remaining >= stream->buf_size) {
		// We want to read more data than the stream buffer will hold. Read
		// directly from the socket into the destination buffer instead.
		size_t copy = remaining - (remaining % stream->buf_size);
		int ret = read(stream->id, dest, copy);
		if (ret < 0) return ret;
		total += ret;
		dest += ret;
		if (ret < copy) return total;
		remaining = bytes - total;
	}
	if (remaining > 0) {
		// The buffer is empty, but we still need more data. Refill the stream
		// buffer, then copy as much of it into the destination buffer as it
		// can still hold.
		int ret = read(stream->id, stream->buf_addr, stream->buf_size);
		if (ret < 0) return ret;
		stream->buf_count = ret;
		if (ret > 0 && ret < stream->buf_size) {
			memmove(buf_end - ret, stream->buf_addr, ret);
		}
		size_t copy = remaining > ret? ret: remaining;
		memcpy(dest, buf_end - stream->buf_count, copy);
		stream->buf_count -= copy;
		total += copy;
	}
	return total;
}

size_t fread(void *dest, size_t size, size_t count, FILE *stream)
{
	size_t bytes = size*count;
	if (0 == bytes) return 0;
	// Must not read from a stream used for writing without flushing first.
	if (stream->state & STREAM_WRITE) {
		errno = -1; // need a better error code here
		stream->state |= STREAM_ERR;
		return 0;
	}
	stream->state |= STREAM_READ;
	int total = 0;
	if (stream->state & STREAM_UNGET) {
		*(unsigned char*)dest = stream->unget;
		dest++;
		stream->state &= ~STREAM_UNGET;
		++total;
		if (0 == --bytes) return total;
	}
	int ret = 0;
	if (stream->buf_size > 0) {
		ret = bufread(stream, dest, bytes);
	} else {
		fflush(0);
		ret = read(stream->id, dest, bytes);
	}
	if (ret < 0) {
		errno = -ret;
		stream->state |= STREAM_ERR;
		return 0;
	} else {
		total += ret;
	}
	if (ret < bytes) {
		stream->state |= STREAM_EOF;
	}
	return total / size;
}

#ifdef TESTSUITE
#include "memsocket.h"

static char prospero[] =
	"Ye elves of hills, brooks, standing lakes and groves,\n"
	"And ye that on the sands with printless foot\n"
	"Do chase the ebbing Neptune and do fly him\n"
	"When he comes back; you demi-puppets that\n"
	"By moonshine do the green sour ringlets make,\n"
	"Whereof the ewe not bites, and you whose pastime\n"
	"Is to make midnight mushrooms, that rejoice\n"
	"To hear the solemn curfew; by whose aid,\n"
	"Weak masters though ye be, I have bedimm'd\n"
	"The noontide sun, calld forth the mutinous winds,\n"
	"And twixt the green sea and the azured vault\n"
	"Set roaring war: to the dread rattling thunder\n"
	"Have I given fire and rifted Joves stout oak\n"
	"With his own bolt; the strong-based promontory\n"
	"Have I made shake and by the spurs pluckd up\n"
	"The pine and cedar: graves at my command\n"
	"Have waked their sleepers, oped, and let 'em forth\n"
	"By my so potent art. But this rough magic\n"
	"I here abjure, and, when I have required\n"
	"Some heavenly music, which even now I do,\n"
	"To work mine end upon their senses that\n"
	"This airy charm is for, I'll break my staff\n"
	"Bury it certain fathoms in the earth,\n"
	"And deeper than did ever plummet sound\n"
	"I'll drown my book.";
static int primes[16] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
static char readbuf[64];
static char destbuf[1536]; // text is just over 1024 bytes long

static struct memsocket ms;
static struct _stream stream;
static void initstream(void)
{
	_init_stream(&stream);
	stream.id = open_memsocket(prospero, strlen(prospero), &ms);
	ms.data_len = ms.buf_size;
	CHECK(ms.data_len == strlen(prospero));
	CHECK(stream.id >= 0);
}
static void resetstream(void)
{
	ms.data_pos = 0;
}
static void exitstream(void)
{
	close(stream.id);
	_exit_stream(&stream);
}

static void run_copy(unsigned buflen, unsigned prime_index, unsigned stride)
{
	resetstream();
	setvbuf(&stream, readbuf, buflen>0?_IOFBF:_IONBF, buflen);
	char *dest = destbuf;
	size_t bytes_left = ms.data_len;
	size_t chunk_size = primes[prime_index];
	while (bytes_left >= chunk_size) {
		CHECK(chunk_size == fread(dest, sizeof(char), chunk_size, &stream));
		if (buflen > 0 && ms.data_pos < ms.data_len) {
			CHECK(ms.data_pos % buflen == 0);
		}
		bytes_left -= chunk_size;
		dest += chunk_size;
		prime_index = (prime_index + stride) % 16;
		chunk_size = primes[prime_index];
	}
	CHECK(bytes_left == fread(dest, sizeof(char), bytes_left, &stream));
	fflush(&stream);
	CHECK(ms.data_pos == ms.buf_size);
	CHECK_MEM(prospero, destbuf, ms.data_len);
}

static void copytest(unsigned buflen)
{
	for (unsigned i = 0; i < 16; ++i) {
		run_copy(buflen, i, 0);
		run_copy(buflen, i, 1);
	}
}

TESTSUITE(fread) {
	initstream();
	// Test the buffered write mechanism by writing this text to a memory
	// stream using a variety of chunk lengths and buffer sizes, using no
	// buffering.
	copytest(0);
	// Repeat the exercise, using a variety of buffer lengths.
	for (unsigned i = 0; i < 16; ++i) {
		copytest(primes[i]);
	}
	exitstream();
}

#endif //TESTSUITE

