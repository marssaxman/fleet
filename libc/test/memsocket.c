#include "memsocket.h"
#include "kernel/include/sys/socket.h"
#include <string.h>

// compiled inside the host environment, not the test environment

static int memsocket_write(void *ref, const void *buf, unsigned bytes)
{
	struct memsocket *ms = (struct memsocket*)ref;
	unsigned avail = ms->buf_size - ms->data_pos;
	unsigned copy = (avail > bytes)? bytes: avail;
	memcpy(ms->buf_addr + ms->data_pos, buf, copy);
	ms->data_pos += copy;
	if (ms->data_pos > ms->data_len) {
		ms->data_len = ms->data_pos;
	}
	return copy;
}

static int memsocket_read(void *ref, void *buf, unsigned bytes)
{
	struct memsocket *ms = (struct memsocket*)ref;
	unsigned avail = ms->data_len - ms->data_pos;
	unsigned copy = (avail > bytes)? bytes: avail;
	memcpy(buf, ms->buf_addr + ms->data_pos, copy);
	ms->data_pos += copy;
	return copy;
}

static struct iops memsocket_ops = {
	.write = memsocket_write,
	.read = memsocket_read,
};

int open_memsocket(void *buf, unsigned size, struct memsocket *ms)
{
	ms->buf_addr = buf;
	ms->buf_size = size;
	ms->data_pos = 0;
	ms->data_len = 0;
	return libc_test_open(ms, &memsocket_ops);
}
