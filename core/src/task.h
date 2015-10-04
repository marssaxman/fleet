#ifndef _TASK_H
#define _TASK_H

// Tasks are units of work which can be joined together into task queues.
// Form a queue by scheduling one task to run before some other task; the
// completion task represents the whole queue, and executing it will execute
// each element in the series. One queue may be inserted into another, as a
// whole, by simply scheduling its completion task before the target queue's
// completion task.

typedef void _task_data;
typedef void (*_task_proc)(_task_data*);
struct _task
{
	_task_proc proc;
	_task_data *data;
	struct _task *tail, *head;
};

void _task_init(struct _task *task, _task_proc proc, _task_data *data);
void _task_schedule(struct _task *queue, struct _task *job);
void _task_execute(struct _task *queue);

#endif //_TASK_H

