#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "list.h"

struct signal_action
{
	struct list_link list;
	void (*proc)(void *);
    void *data;
};
void action_init(struct signal_action*, void (*proc)(void*), void *data);

struct signal
{
	struct list_link actions;
};
// One-time initialization readying a signal for use.
void signal_init(struct signal*);

// Queue this action to be executed after the signal is raised.
void signal_listen(struct signal*, struct signal_action*);

// Raise the signal, execute its queued actions, then reset its queue.
void signal_raise(struct signal*);

// Reset the signal, clearing its action queue.
void signal_reset(struct signal*);

// Clear the 'now' queue and move all of its actions over to 'later'.
void signal_defer(struct signal *now, struct signal *later);

#endif //_SIGNAL_H

