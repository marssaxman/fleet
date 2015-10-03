#include "task.h"

static void do_nothing(struct _task *job) {}

void _task_init(struct _task *job)
{
	// All tasks must be part of a circular queue, so a blank task structure
	// needs to be initialized with pointers to itself, a one-element queue.
	job->next = job;
	job->previous = job;
	job->proc = do_nothing;
}

void _task_schedule(struct _task *queue, struct _task *job)
{
	// Link the first item in the job queue to the penultimate item in the
	// destination queue and vice versa. This inserts the job queue after the
	// other tasks that were already going to be performed on this queue.
	job->next->previous = queue->previous;
	queue->previous->next = job->next;

	// Link the job's final task in just before the queue's final task.
	job->next = queue;
	queue->previous = job;
}

void _task_execute(struct _task *queue)
{
	do {
		// The first task to execute is always at the head of the queue.
		struct _task *task = queue->next;
		task->proc(task);
		// Remove the completed task from the queue. The task may have added
		// new items to the queue so we can't assume it is still the head.
		task->next->previous = task->previous;
		task->previous->next = task->next;
		// Wrap the task's ends back around so it remains usable in future.
		task->next = task->previous = task;
		// The job ends after we have come all the way around and finished
		// the final entry, which is of course 'queue' itself.
	} while (queue->next != queue);
}

