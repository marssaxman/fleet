#include "events.h"

static void events_done(struct _task *ignored) {}
static struct _task event_loop = {.proc = (_task_proc)events_done};

void defer(struct _task *event)
{
	_task_schedule(&event_loop, event);
}

void poll()
{
	_task_execute(&event_loop);
}
