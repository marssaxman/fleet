#ifndef _TASK_H
#define _TASK_H

// Tasks are units of work which can be scheduled for later execution by
// adding them to a task queue.

typedef void _task_data;
typedef void (*_task_proc)(_task_data*);
struct _task
{
	_task_proc proc;
	_task_data *data;
	struct _task *next, **chain;
};

struct _task_queue
{
	struct _task *head, *tail;
};

void _task_init(struct _task *task, _task_proc proc, _task_data *data);
void _task_schedule(struct _task_queue*, struct _task*);
void _task_execute(struct _task_queue*);

#endif //_TASK_H

