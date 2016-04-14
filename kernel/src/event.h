// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef EVENT_H
#define EVENT_H

struct event {
	void (*handler)(struct event*);
	struct event *_prev, *_next;
};
void _event_signal(struct event*);

#endif //EVENT_H

