#include <sys/errno.h>
#include <stddef.h>
#include "socket.h"
#include "panic.h"

struct socket {
	void *ref;
	const struct iops *ops;
};

#define SOCKET_MAX 16 // arbitrary
static struct socket _sockets[SOCKET_MAX];

socket_t _open(void *ref, const struct iops *ops)
{
	for (int i = 0; i < SOCKET_MAX; ++i) {
		struct socket *s = &_sockets[i];
		if (0 == s->ops) {
			s->ref = ref;
			s->ops = ops;
			return i;
		}
	}
	return EMFILE;
}

int transmit(socket_t id, struct transfer *xfer)
{
	if (id < 0 || id >= SOCKET_MAX) return EBADF;
	struct socket *s = &_sockets[id];
	if (0 == s->ops) return EBADF;
	if (0 == s->ops->transmit) return EPERM;
	return s->ops->transmit(s->ref, xfer);
}

int receive(socket_t id, struct transfer *xfer)
{
	if (id < 0 || id >= SOCKET_MAX) return EBADF;
	struct socket *s = &_sockets[id];
	if (0 == s->ops) return EBADF;
	if (0 == s->ops->receive) return EPERM;
	return s->ops->receive(s->ref, xfer);
}

int sync(socket_t id)
{
	if (id < 0 || id >= SOCKET_MAX) return EBADF;
	struct socket *s = &_sockets[id];
	if (0 == s->ops) return EBADF;
	if (0 == s->ops->sync) return 0;
	return s->ops->sync(s->ref);
}

int close(int id)
{
	if (id < 0 || id >= SOCKET_MAX) return EBADF;
	struct socket *s = &_sockets[id];
	if (0 == s->ops) return EBADF;
	int out = 0;
	if (0 == s->ops->sync) out = s->ops->sync(s->ref);
	if (0 == s->ops->close) s->ops->close(s->ref);
	s->ops = 0;
	return out;
}

