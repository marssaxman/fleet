// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"
#include "syscalls.h"
#include <errno.h>
#include <string.h>

static int bufwrite(FILE *stream, const char *src, size_t bytes)
{
	size_t total = 0;
	if (stream->buf_count > 0 && stream->buf_size > stream->buf_count) {
		// There's data in the stream buffer, so we need to write it out before
		// any of the new stuff we're adding, but there's still space in the
		// buffer. Copy in as much of the source data as will fit.
		size_t remain = bytes - total;
		size_t avail = stream->buf_size - stream->buf_count;
		size_t copy = (avail > remain)? remain: avail;
		memcpy(stream->buf_addr + stream->buf_count, src, copy);
		stream->buf_count += copy;
		src += copy;
		total += copy;
	}
	if (stream->buf_count == stream->buf_size) {
		// The stream buffer is full, so we have to flush it before we can
		// queue up any more outgoing data.
		int expect = stream->buf_size;
		stream->buf_count = 0;
		int ret = write(stream->id, stream->buf_addr, expect);
		if (ret < 0) return ret;
		if (ret < expect) return total;
	}
	if (bytes - total >= stream->buf_size) {
		// The data remaining to write is bigger than the stream buffer, so
		// we might as well copy it directly out from the source buffer rather
		// than wasting time copying it into the stream buffer first.
		size_t remaining = bytes - total;
		size_t copy = remaining - (remaining % stream->buf_size);
		int ret = write(stream->id, src, copy);
		if (ret < 0) return ret;
		total += ret;
		src += ret;
		if (ret < copy) return total;
	}
	if (bytes > total) {
		// We have some bytes left, not enough to fill the stream buffer. Copy
		// them in and leave them to write out next time.
		size_t avail = stream->buf_size - stream->buf_count;
		size_t remain = bytes - total;
		size_t copy = (avail > remain)? remain: avail;
		memcpy(stream->buf_addr + stream->buf_count, src, copy);
		stream->buf_count += copy;
		src += copy;
		total += copy;
	}
	return bytes;
}

static int linewrite(FILE *stream, const char *src, size_t bytes)
{
	size_t total = 0;
	char *dest = stream->buf_addr + stream->buf_count;
	while (total < bytes) {
		if (stream->buf_count < stream->buf_size) {
			char c = *src++;
			*dest++ = c;
			stream->buf_count++;
			total++;
			if (c != '\n') continue;
		}
		int ret = write(stream->id, stream->buf_addr, stream->buf_count);
		if (ret < 0) return ret;
		if (ret < stream->buf_count) break;
		stream->buf_count = 0;
		dest = stream->buf_addr;
	}
	return total;
}

size_t fwrite(const void *src, size_t size, size_t count, FILE *stream)
{
	size_t bytes = size*count;
	if (0 == bytes) return 0;
	// Must not write to a stream used for reading without an intervening
	// seek/setpos/rewind.
	if (stream->state & STREAM_READ) {
		errno = -1; //??
		stream->state |= STREAM_ERR;
		return 0;
	}
	stream->state |= STREAM_WRITE;
	int ret = 0;
	if (stream->buf_size > 0) {
		if (stream->state & STREAM_LINESYNC) {
			ret = linewrite(stream, src, bytes);
		} else {
			ret = bufwrite(stream, src, bytes);
		}
	} else {
		ret = write(stream->id, src, bytes);
	}
	if (ret < bytes) {
		errno = (ret < 0)? -ret: -1; //???
		stream->state |= STREAM_ERR;
		return 0;
	}
	return ret / size;
}

#ifdef TESTSUITE
#include "memsocket.h"

static char streambuf[1024];
static struct memsocket ms;
struct _stream stream;
static void initstream(void)
{
	_init_stream(&stream);
	ms.data_len = 0;
	stream.id = open_memsocket(streambuf, 1024, &ms);
	CHECK(stream.id >= 0);
}
static void resetstream(void)
{
	ms.data_len = 0;
	ms.data_pos = 0;
}
static void exitstream(void)
{
	close(stream.id);
	_exit_stream(&stream);
}

static char the_tempest[] =
	"Our revels now are ended. These our actors,\n"
	"As I foretold you, were all spirits and\n"
	"Are melted into air, into thin air:\n"
	"And, like the baseless fabric of this vision,\n"
	"The cloud-cappd towers, the gorgeous palaces,\n"
	"The solemn temples, the great globe itself,\n"
	"Yea, all which it inherit, shall dissolve\n"
	"And, like this insubstantial pageant faded,\n"
	"Leave not a rack behind. We are such stuff\n"
	"As dreams are made on, and our little life\n"
	"Is rounded with a sleep.\n";
size_t the_tempest_len;

static int primes[16] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
static char writebuf[64];

static void run_copy(unsigned buflen, unsigned prime_index, unsigned stride)
{
	resetstream();
	setvbuf(&stream, writebuf, buflen>0?_IOFBF:_IONBF, buflen);
	const char *src = the_tempest;
	size_t bytes_left = the_tempest_len;
	size_t chunk_size = primes[prime_index];
	while (bytes_left >= chunk_size) {
		CHECK(chunk_size == fwrite(src, sizeof(char), chunk_size, &stream));
		CHECK(buflen == 0 || ms.data_len % buflen == 0);
		bytes_left -= chunk_size;
		src += chunk_size;
		prime_index = (prime_index + stride) % 16;
		chunk_size = primes[prime_index];
	}
	fwrite(src, sizeof(char), bytes_left, &stream);
	fflush(&stream);
	CHECK(ms.data_len == the_tempest_len);
	CHECK_MEM(ms.buf_addr, the_tempest, the_tempest_len);
}

static void copytest(unsigned buflen)
{
	for (unsigned i = 0; i < 16; ++i) {
		run_copy(buflen, i, 0);
		run_copy(buflen, i, 1);
	}
}

static void run_line(unsigned buflen, unsigned prime_index, unsigned stride)
{
	resetstream();
	CHECK(buflen > 0);
	setvbuf(&stream, writebuf, _IOLBF, buflen);
	const char *src = the_tempest;
	size_t bytes_left = the_tempest_len;
	size_t chunk_size = primes[prime_index];
	while (bytes_left >= chunk_size) {
		CHECK(chunk_size == fwrite(src, sizeof(char), chunk_size, &stream));
		bytes_left -= chunk_size;
		src += chunk_size;
		prime_index = (prime_index + stride) % 16;
		chunk_size = primes[prime_index];
	}
	fwrite(src, sizeof(char), bytes_left, &stream);
	fflush(&stream);
	CHECK(ms.data_len == the_tempest_len);
	CHECK_MEM(ms.buf_addr, the_tempest, the_tempest_len);
}

static void linetest(unsigned buflen)
{
	for (unsigned i = 0; i < 16; ++i) {
		run_line(buflen, i, 0);
		run_line(buflen, i, 1);
	}
}

TESTSUITE(fwrite) {
	initstream();
	the_tempest_len = strlen(the_tempest);
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
#endif
