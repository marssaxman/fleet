# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global ring_init, ring_empty, ring_push, ring_pop, ring_pull

.set LIST_HEAD, 0
.set LIST_TAIL, 4
.set ITEM_NEXT, 0
.set ITEM_PREV, 4

ring_init:
	# ring starts out pointing at itself from both ends
	movl 4(%esp), %eax
	movl %eax, LIST_HEAD(%eax)
	movl %eax, LIST_TAIL(%eax)
	ret

ring_push:
	# add item to end of list
	movl 4(%esp), %eax # list
	movl 8(%esp), %ecx # item
	pushf
	cli
	movl %eax, ITEM_NEXT(%ecx)
	movl LIST_TAIL(%eax), %edx
	movl %edx, ITEM_PREV(%ecx)
	movl %ecx, ITEM_NEXT(%edx)
	movl %eax, LIST_TAIL(%eax)
	popf
	ret

ring_pop:
	# remove and return the last item in the list
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

ring_pull:
	# remove and return the first item in the list
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

ring_remove:
	# detach this item from the links on either side
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

