#include <workqueue.h>

void work_item_init(struct work_item *work, void(*proc)(void*), void *data)
{
	LIST_LINK_INIT(&work->list);
	work->proc = proc;
	work->data = data;
}

void work_queue_init(struct work_queue *queue)
{
	LIST_LINK_INIT(&queue->items);
}

void queue_work(struct work_queue *queue, struct work_item *work)
{
	list_move_before(&work->list, &queue->items);
}

void await_queue(struct work_queue *queue)
{
	unsigned i = 0;
	struct list_link *link;
	while (link = list_pop(&queue->items)) {
		struct work_item *item = LIST_ENTRY(link, struct work_item, list);
		item->proc(item->data);
	}
}
