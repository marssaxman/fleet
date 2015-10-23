#ifndef _LIBC_STREAM_H
#define _LIBC_STREAM_H

struct _libc_stream
{
	void *ref;
	size_t (*write)(void *ref, const char *src, size_t bytes);
	size_t (*read)(void *ref, char *dest, size_t max);
	// close
	// seek/tell
	// status
	// flush
};

#endif //_LIBC_STREAM_H

