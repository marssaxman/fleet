#include "events.h"

static struct signal events;

void events_init()
{
	signal_init(&events);
}

void event_action(struct signal_action *action)
{
	signal_listen(&events, action);
}

void event_signal(struct signal *signal)
{
	signal_defer(signal, &events);
}

void poll_events()
{
	signal_raise(&events);
}

