#include "memsocket.h"
#include "kernel/include/sys/socket.h"
#include <string.h>

// compiled inside the host environment, not the test environment

static int memsocket_write(void *ref, const void *buf, unsigned bytes)
{
	struct memsocket *ms = (struct memsocket*)ref;
	unsigned avail = ms->buf_size - ms->data_len;
	unsigned copy = (avail > bytes)? bytes: avail;
	memcpy(ms->buf_addr + ms->data_len, buf, copy);
	ms->data_len += copy;
	return copy;
}

static struct iops memsocket_ops = {
	.write = memsocket_write,
};

int open_memsocket(struct memsocket *ms)
{
	return libc_test_open(ms, &memsocket_ops);
}
