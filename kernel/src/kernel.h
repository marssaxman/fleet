// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef KERNEL_H
#define KERNEL_H

void _poll();

// A task is a single unit of work: a function and its associated data.
struct task {
	void (*method)(struct task*);
	struct task *task_queue, **task_link;
};

// A process is a sequence of related tasks.
struct process {
	struct task *task_queue, **task_link;
	struct process *process_queue, **process_link;
};
void _start(struct process*);
void _stop(struct process*);
void _schedule(struct process*, struct task*);

// Posting an event schedules its handler onto the context process' queue.
struct event {
	struct process *context;
	struct task *handler;
};
void _post(struct event*);

#endif //KERNEL_H

