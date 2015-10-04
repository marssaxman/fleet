#include "events.h"

static struct _task_queue event_loop;

void defer(struct _task *event)
{
	_task_schedule(&event_loop, event);
}

void poll()
{
	_task_execute(&event_loop);
}
