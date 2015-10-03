#include "task.h"

// Life is a lot simpler if we pretend that single tasks are actually just
// single-item queues, so we'll represent NULL links with the task itself.
static inline struct _task *next(struct _task *t)
{
	return t->next? t->next: t;
}

static inline struct _task *previous(struct _task *t)
{
	return t->previous? t->previous: t;
}

void _task_schedule(struct _task *queue, struct _task *job)
{
	// Link the first item in the job queue to the penultimate item in the
	// destination queue and vice versa. This inserts the job queue after the
	// other tasks that were already going to be performed on this queue.
	next(job)->previous = previous(queue);
	previous(queue)->next = next(job);

	// Link the job's final task in just before the queue's final task.
	job->next = queue;
	queue->previous = job;
}

void _task_execute(struct _task *queue)
{
	do {
		// The first task to execute is always at the head of the queue.
		struct _task *task = next(queue);
		task->proc(task);
		// Remove the completed task from the queue. The task may have added
		// new items to the queue so we can't assume it is still the head.
		next(task)->previous = previous(task);
		previous(task)->next = next(task);
		// Wrap the task's ends back around so it remains usable in future.
		task->next = task->previous = task;
		// The job ends after we have come all the way around and finished
		// the final entry, which is of course 'queue' itself.
	} while (next(queue) != queue);
}

