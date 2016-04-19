// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _FLEET_SYSTEM_H
#define _FLEET_SYSTEM_H

struct task {
	void (*method)(struct task*);
	struct task *next;
};

void post(struct task*);
void yield();

#endif //_FLEET_SYSTEM_H


