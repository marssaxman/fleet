#include <fleet/signal.h>

void action_init(struct signal_action *action, void(*proc)(void*), void *data)
{
	LIST_LINK_INIT(&action->list);
	action->proc = proc;
	action->data = data;
}

void signal_init(struct signal *signal)
{
	LIST_LINK_INIT(&signal->actions);
}

void signal_listen(struct signal *signal, struct signal_action *action)
{
	list_move_before(&action->list, &signal->actions);
}

void signal_raise(struct signal *signal)
{
	unsigned i = 0;
	struct list_link *link;
	while (link = list_pop(&signal->actions)) {
		struct signal_action *action;
		action = LIST_ENTRY(link, struct signal_action, list);
		action->proc(action->data);
	}
}

void signal_reset(struct signal *signal)
{
	while (list_pop(&signal->actions)) {}
}

void signal_defer(struct signal *now, struct signal *later)
{
	list_splice(&now->actions, &later->actions);
}


