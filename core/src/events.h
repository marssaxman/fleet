#ifndef _EVENTS_H
#define _EVENTS_H

// Device drivers handle IRQs by inspecting device state and determining
// what caused the interrupt, but we want to return from the ISR as soon as
// possible so we can re-enable interrupts. The driver should therefore defer
// substantial work by scheduling a task on the event queue.

#include <fleet/signal.h>

void events_init();
void event_action(struct signal_action *action);
void event_signal(struct signal *signal);
void poll_events();

#endif //_EVENTS_H
