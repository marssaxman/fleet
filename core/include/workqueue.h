#ifndef _WORKQUEUE_H
#define _WORKQUEUE_H

#include <list.h>

struct work_item
{
	struct list_link list;
	void (*proc)(void *);
    void *data;
};
void work_item_init(struct work_item*, void (*proc)(void*), void *data);

struct work_queue
{
	struct list_link items;
};
void work_queue_init(struct work_queue*);
void queue_work(struct work_queue*, struct work_item*);
void await_queue(struct work_queue*);

#endif //_SYS_WORKQUEUE_H
