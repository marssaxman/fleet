# Copyright (C) 2016 Mars Saxman. All rights reserved.
# Permission is granted to use at your own risk and distribute this software
# in source and binary forms provided all source code distributions retain
# this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
# IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

.global ring_init, ring_push, ring_pop, ring_pull

.set LIST_HEAD, 0
.set LIST_TAIL, 4
.set ITEM_NEXT, 0
.set ITEM_PREV, 4

ring_init:
	movl 4(%esp), %eax
	movl %eax, (%eax)
	movl %eax, 4(%eax)
	ret

ring_push:
	movl 4(%esp), %eax # list
	movl 8(%esp), %ecx # item
	pushf
	cli
	movl %eax, ITEM_NEXT(%ecx)
	movl ITEM_PREV(%eax), %edx
	movl %edx, ITEM_PREV(%ecx)
	movl %ecx, ITEM_NEXT(%edx)
	popf
	ret

ring_pop:
	movl 4(%esp), %ecx # list
	pushf
	cli
	xorl %eax, %eax
	cmpl ITEM_PREV(%ecx), %ecx
	je 0f
	movl ITEM_PREV(%ecx), %eax # old tail
	movl ITEM_PREV(%eax), %edx # new tail
	movl %ecx, ITEM_NEXT(%edx)
	movl %edx, ITEM_PREV(%ecx)
	movl %eax, ITEM_NEXT(%eax)
	movl %eax, ITEM_PREV(%eax)
0:	popf
	ret

ring_pull:
	movl 4(%esp), %ecx # list
	pushf
	cli
	xorl %eax, %eax
	cmpl ITEM_NEXT(%ecx), %ecx
	je 0f
	movl ITEM_NEXT(%ecx), %eax # item to remove
	movl ITEM_NEXT(%eax), %edx # new head item
	movl %ecx, ITEM_PREV(%edx)
	movl %edx, ITEM_NEXT(%ecx)
	movl %eax, ITEM_PREV(%eax)
	movl %eax, ITEM_NEXT(%eax)
0:	popf
	ret




