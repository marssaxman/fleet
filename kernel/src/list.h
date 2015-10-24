#ifndef LIST_H
#define LIST_H

// Tools managing doubly-linked circular lists as used in virtually every
// kernel subsystem.

struct list_link
{
	struct list_link *next;
	struct list_link *prev;
};

// An empty list should point back at itself from both ends. Initialize new
// list links with these macros.
#define LIST_INITIALIZER(name) {&(name), &(name)}
#define LIST_LINK(name) struct list_link name = LIST_INITIALIZER(name)
#define LIST_LINK_INIT(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

// Retrieve the struct that contains this link, given the address of the link
// and its member name within the containing struct.
#define LIST_ENTRY(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

// Move the item into the list following 'prev'.
void list_move_after(struct list_link *item, struct list_link *prev);

// Move the item into the list preceding 'next'.
void list_move_before(struct list_link *item, struct list_link *next);

// Remove the item from the list, reinitializing the link.
void list_remove(struct list_link *item);

// Remove and return the next item from this list.
struct list_link *list_pop(struct list_link *item);

// Does this link represent an empty list, with no other items?
int list_empty(struct list_link *item);

// Insert all items belonging to 'list' after the link 'prev'.
void list_splice(struct list_link *list, struct list_link *prev);

// Iterate through the list, following ->next links forward.
#define list_iter_next(pos, list) \
	for (pos = (list)->next; pos != (list); pos = pos->next)

// Iterate through the list, following ->prev links backward.
#define list_iter_prev(pos, list) \
	for (pos = (list)->prev; pos != (list); pos = pos->prev)

#endif // LIST_H
