#include "task.h"

void _task_init(struct _task *task, _task_proc proc, _task_data *data)
{
	task->chain = 0;
	task->next = 0;
	task->proc = proc;
	task->data = data;
}

void _task_schedule(struct _task_queue *queue, struct _task *task)
{
	// If the task is already part of a queue, remove it. 
	if (task->chain) {
		*task->chain = task->next;
	}
	if (task->next) {
		task->next->chain = task->chain;
		task->next = 0;
	}
	// If the queue is empty, this becomes both head and tail. Otherwise,
	// link it in at the tail of the chain.
	task->chain = queue->tail? &queue->tail->next: &queue->head;
	queue->tail = *task->chain = task;
}

void _task_execute(struct _task_queue *queue)
{
	while (queue->head) {
		struct _task *task = queue->head;
		*task->chain = task->next;
		task->proc(task->data);
	}
	queue->tail = queue->head;
}

