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
static int bad_sync(void *r) { return -EBADF; }
static int bad_read(void *r, void *d, unsigned n) { return -EBADF; }
static int bad_write(void *r, const void *s, unsigned n) { return -EBADF; }
static struct iops bad_ops = {
	.sync = bad_sync,
	.write = bad_write,
	.read = bad_read,
};

#define STREAM_MAX 16
static struct stream _streams[STREAM_MAX] = {
	{ .ref = 0, .ops = &bad_ops },
};

static inline struct stream *lookup(int id)
{
	if (id < 0 || id >= STREAM_MAX) return _streams + 0;
	struct stream *s = _streams + id;
	return s->ops? s: _streams + 0;
}

int _stream_open(void *ref, struct iops *ops)
{
	for (int i = 1; i < STREAM_MAX; ++i) {
		struct stream *s = _streams + i;
		if (0 == s->ops) {
			s->ref = ref;
			s->ops = ops;
			return i;
		}
	}
	return -EMFILE;
}

int write(int id, const void *src, unsigned bytes)
{
	struct stream *s = lookup(id);
	if (!s->ops->write) return -EPERM;
	return s->ops->write(s->ref, src, bytes);
}

int read(int id, void *dest, unsigned bytes)
{
	struct stream *s = lookup(id);
	if (!s->ops->read) return -EPERM;
	return s->ops->read(s->ref, dest, bytes);
}

int close(int id)
{
	struct stream *s = lookup(id);
	if (s->ops == &bad_ops) return -EBADF;
	if (s->ops->close) s->ops->close(s->ref);
	s->ops = 0;
	return 0;
}

int sync(int id)
{
	struct stream *s = lookup(id);
	if (!s->ops->sync) return -EINVAL;
	return s->ops->sync(s->ref);
}

