#ifndef _TASK_H
#define _TASK_H

// A task is a piece of work that can be scheduled on a queue. After all
// preceding tasks have been performed, the task's proc will be invoked.
struct _task
{
	struct _task *previous, *next;
	void (*proc)(struct _task*);
};
void _task_init(struct _task *job);
void _task_schedule(struct _task *queue, struct _task *job);
void _task_execute(struct _task *queue);

#endif //_TASK_H

