#include "task.h"

// Life is a lot simpler if we pretend that single tasks are actually just
// single-item queues, so we'll represent NULL links with the task itself.
static inline struct _task *head(struct _task *t)
{
	return t->head? t->head: t;
}

static inline struct _task *tail(struct _task *t)
{
	return t->tail? t->tail: t;
}

static inline void link(struct _task *tail, struct _task *head)
{
	tail->head = head;
	head->tail = tail;
}

static inline struct _task *pull(struct _task *queue)
{
	struct _task *task = head(queue);
	link(tail(task), head(task));
	task->head = task->tail = 0;
	return task;
}

static inline void exec(struct _task *task)
{
	task->proc(task->data);
}

static inline int more(struct _task *queue)
{
	return head(queue) != queue;
}

void _task_init(struct _task *task, _task_proc proc, _task_data *data)
{
	task->proc = proc;
	task->data = data;
	task->head = task;
	task->tail = task;
}

void _task_schedule(struct _task *queue, struct _task *job)
{
	// Link the first item in the job queue to the penultimate item in the
	// destination queue and vice versa. This inserts the job queue after the
	// other tasks that were already going to be performed on this queue.
	link(tail(queue), head(job));

	// Link the job's final task in just before the queue's final task.
	job->head = queue;
	queue->tail = job;
}

void _task_execute(struct _task *queue)
{
	do exec(pull(queue)); while (more(queue));
}

