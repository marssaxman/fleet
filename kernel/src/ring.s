# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global ring_init, ring_push, ring_put, ring_pop, ring_pull

.set LIST_HEAD, 0
.set LIST_TAIL, 4
.set ITEM_NEXT, 0
.set ITEM_PREV, 4

# void ring_init(struct ring_list*);
# A ring starts out pointing at itself from both ends.
ring_init: .global ring_init
	movl 4(%esp), %eax
	movl %eax, LIST_HEAD(%eax)
	movl %eax, LIST_TAIL(%eax)
	ret

# bool ring_empty(struct ring_list*);
# Is this ring currently pointing at itself from both ends?
ring_empty: .global ring_empty
	xorl %eax, %eax
	movl 4(%esp), %ecx
	cmpl %ecx, LIST_HEAD(%ecx)
	jne 0f
	cmpl %ecx, LIST_TAIL(%ecx)
	sete %al
0:	ret

# struct ring_item *ring_head(struct ring_list*);
# Get the first item in the list, if any.
ring_head: .global ring_head
	movl 4(%esp), %ecx
	movl LIST_HEAD(%ecx), %eax
	cmpl %ecx, %eax
	jne 0f
	xorl %eax, %eax
0:	ret

# struct ring_item *ring_tail(struct ring_list*);
# Get the last item in the list, if any.
ring_tail: .global ring_tail
	movl 4(%esp), %ecx
	movl LIST_TAIL(%ecx), %eax
	cmpl %ecx, %eax
	jne 0f
	xorl %eax, %eax
0:	ret

# struct ring_item *ring_next(struct ring_list*, struct ring_item*);
# Get the item following the current one.
ring_next: .global ring_next
	movl 4(%esp), %ecx
	movl 8(%esp), %eax
	movl ITEM_NEXT(%eax), %eax
	cmpl %ecx, %eax
	jne 0f
	xorl %eax, %eax
0:	ret

# struct ring_item *ring_prev(struct ring_list*, struct ring_item*);
# Get the item preceding the current one.
ring_prev: .global ring_prev
	movl 4(%esp), %ecx
	movl 8(%esp), %eax
	movl ITEM_PREV(%eax), %eax
	cmpl %ecx, %eax
	jne 0f
	xorl %eax, %eax
0:	ret

# void ring_put(struct ring_list*, struct ring_item*);
# Insert an item at the head of this list.
ring_put: .global ring_put
	movl 4(%esp), %eax # list
	movl 8(%esp), %ecx # item
	pushf
	cli
	movl %eax, ITEM_PREV(%ecx) # item->prev = list
	movl LIST_HEAD(%eax), %edx # next = list->head
	movl %edx, ITEM_NEXT(%ecx) # item->next = next
	movl %ecx, ITEM_PREV(%edx) # next->prev = item
	movl %ecx, LIST_HEAD(%eax) # list->head = item
	popf
	ret

# void ring_push(struct ring_list*, struct ring_item*);
# Insert an item at the tail of this list.
ring_push: .global ring_push
	movl 4(%esp), %eax # list
	movl 8(%esp), %ecx # item
	pushf
	cli
	movl %eax, ITEM_NEXT(%ecx) # item->next = list
	movl LIST_TAIL(%eax), %edx # prev = list->tail
	movl %edx, ITEM_PREV(%ecx) # item->prev = prev
	movl %ecx, ITEM_NEXT(%edx) # prev->next = item
	movl %ecx, LIST_TAIL(%eax) # list->tail = item
	popf
	ret

# struct ring_item* ring_pull(struct ring_list*);
# Remove the head and return it.
ring_pull: .global ring_pull
	movl 4(%esp), %ecx # list
	pushf
	cli
	# Get the first item in the list. If it is equal to the list item itself,
	# that is the sentinel value indicating that the list is empty.
	movl LIST_HEAD(%ecx), %eax
	cmpl %eax, %ecx
	je .L_empty_return
	# Link the head's next item to the list, thereby removing the old head.
	movl ITEM_NEXT(%eax), %edx
	movl %edx, LIST_HEAD(%ecx)
	movl %ecx, ITEM_PREV(%edx)
	jmp .L_detach_return

# struct ring_item* ring_pop(struct ring_list*);
# Remove the tail and return it.
ring_pop: .global ring_pop
	movl 4(%esp), %ecx # list
	pushf
	cli
	# Get the last item in the list. If it is equal to the list item itself,
	# that is the sentinel value showing that the list is empty.
	movl LIST_TAIL(%ecx), %eax
	cmpl %eax, %ecx
	je .L_empty_return
	# Link the tail's previous item to the list, removing the old tail.
	movl ITEM_PREV(%eax), %edx
	movl %edx, LIST_TAIL(%ecx)
	movl %ecx, ITEM_NEXT(%edx)
	jmp .L_detach_return

# void ring_remove(struct ring_item*);
# remove this item from whatever group it is part of.
ring_remove: .global ring_remove
	movl 4(%esp), %eax
	pushf
	cli
	movl ITEM_PREV(%eax), %ecx
	movl ITEM_NEXT(%eax), %edx
	movl %ecx, ITEM_PREV(%edx)
	movl %edx, ITEM_NEXT(%ecx)
	jmp .L_detach_return

.L_detach_return:
	# Clear the item's links to show that it is no longer attached to a ring.
	xorl %ecx, %ecx
	movl %ecx, ITEM_NEXT(%eax)
	movl %ecx, ITEM_PREV(%eax)
	popf
	ret

.L_empty_return:
	# The list contains no items. Return zero.
	xorl %eax, %eax
	popf
	ret

