// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

struct args {
	// number of defined entries
	size_t count;
	// array of argument values
	char **vec;
	// number of allocated entries in the array
	size_t size;
};

void args_init(struct args*);
void args_exit(struct args*);
void args_push(struct args*, char*);
void args_push2(struct args*, char*, char*);

#endif //ARGS_H
