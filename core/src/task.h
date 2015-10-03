#ifndef _TASK_H
#define _TASK_H

// Tasks are units of work which can be joined together into task queues.
// Form a queue by scheduling one task to run before some other task; the
// completion task represents the whole queue, and executing it will execute
// each element in the series. One queue may be inserted into another, as a
// whole, by simply scheduling its completion task before the target queue's
// completion task.

struct _task
{
	struct _task *tail, *head;
	void (*proc)(struct _task*);
};
void _task_schedule(struct _task *queue, struct _task *job);
void _task_execute(struct _task *queue);

#endif //_TASK_H

