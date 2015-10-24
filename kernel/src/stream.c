#include <sys/syscall.h>
#include <sys/errno.h>
#include "stream.h"
#include "panic.h"

struct stream {
	void *ref;
	struct iops *ops;
};

// Reserve stream id 0 for a canonical bad stream, which always returns EBADF.
// This way we can map all out-of-bounds streamids to a real stream record and
// not have to do range or null checks everywhere.
static int badread(void *r, void *d, unsigned n) { return -EBADF; }
static int badwrite(void *r, const void *s, unsigned n) { return -EBADF; }
static int badclose(void *r) { return -EBADF; }
static struct iops badiops = {
	.write = badwrite,
	.read = badread,
	.close = badclose,
};

#define STREAM_MAX 16
static struct stream _streams[STREAM_MAX] = {
	{ .ref = 0, .ops = &badiops },
};

static inline struct stream *lookup(int id)
{
	if (id < 0 || id >= STREAM_MAX) return _streams + 0;
	struct stream *s = _streams + id;
	return s->ops? s: _streams + 0;
}

int _stream_open(void *ref, struct iops *ops)
{
	for (int i = 0; i < STREAM_MAX; ++i) {
		struct stream *s = _streams + i;
		if (0 == s->ops) {
			s->ref = ref;
			s->ops = ops;
			return i;
		}
	}
	return -EMFILE;
}

void _stream_release(int streamid)
{
	assert(streamid >=0 && streamid < STREAM_MAX);
	_streams[streamid].ops = 0;
}

int write(int id, const void *src, unsigned bytes)
{
	struct stream *s = lookup(id);
	return s->ops->write? s->ops->write(s->ref, src, bytes): -EPERM;
}

int read(int id, void *dest, unsigned bytes)
{
	struct stream *s = lookup(id);
	return s->ops->read? s->ops->read(s->ref, dest, bytes): -EPERM;
}

int close(int id)
{
	struct stream *s = lookup(id);
	return s->ops->close? s->ops->close(s->ref): -EPERM;
}
