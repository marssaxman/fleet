#ifndef STREAM_H
#define STREAM_H

struct iops
{
	void (*close)(void *ref);
	int (*sync)(void *ref);
	int (*read)(void *ref, void *buf, unsigned bytes);
	int (*write)(void *ref, const void *buf, unsigned bytes);
};

// Defined in sys/syscall.h, implemented in stream.c
// int close(int streamid);
// int sync(int streamid);
// int write(int streamid, const void *src, unsigned bytes);
// int read(int streamid, void *dest, unsigned bytes);

int _stream_open(void *ref, struct iops*);

#endif //STREAM_H
