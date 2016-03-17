// Copyright (C) 2015-2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "list.h"

void list_move_after(struct list_link *item, struct list_link *base)
{
	list_remove(item);
	struct list_link *prev = base->prev;
	struct list_link *next = base;
	next->prev = item;
	item->next = next;
	item->prev = prev;
	prev->next = item;
}

void list_move_before(struct list_link *item, struct list_link *base)
{
	list_remove(item);
	struct list_link *prev = base;
	struct list_link *next = base->next;
	next->prev = item;
	item->next = next;
	item->prev = prev;
	prev->next = item;
}

void list_remove(struct list_link *item)
{
	struct list_link *prev = item->prev;
	struct list_link *next = item->next;
	next->prev = prev;
	prev->next = next;
	item->next = item;
	item->prev = item;
}

int list_empty(struct list_link *item)
{
	return item->next == item;
}

struct list_link *list_pop(struct list_link *item)
{
	if (list_empty(item)) return 0;
	struct list_link *head = item->next;
	list_remove(head);
	return head;
}

void list_splice(struct list_link *list, struct list_link *head)
{
	if (list_empty(list)) return;
	struct list_link *first = list->next;
	struct list_link *last = list->prev;
	struct list_link *at = head->next;
	first->prev = head;
	head->next = first;
	last->next = at;
	at->prev = last;
	list->next = list;
	list->prev = list;
}



