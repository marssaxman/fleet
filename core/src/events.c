#include "events.h"

static struct work_queue events;

void events_init()
{
	work_queue_init(&events);
}

void raise_event(struct work_item *event)
{
	queue_work(&events, event);
}

void poll_events()
{
	await_queue(&events);
}

