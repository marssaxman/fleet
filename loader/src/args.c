// Copyright (C) 2016 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "args.h"
#include <string.h>
#include <stdlib.h>

// Array of const char* with terminating null.

void args_init(struct args *a) {
	a->count = 0;
	a->size = 1;
	a->vec = calloc(1, sizeof(char*));
}

void args_exit(struct args *a) {
	if (a->vec) {
		free(a->vec);
		a->vec = NULL;
	}
}

static void prepare_space(struct args *a, size_t more_items) {
	size_t newcount = a->count + more_items;
	size_t newsize = a->size;
	while (newcount >= newsize) {
		newsize <<= 1;
	}
	if (newsize > a->size) {
		a->size = newsize;
		char **newargs = calloc(newsize, sizeof(char*));
		memcpy(newargs, a->vec, a->count * sizeof(char*));
		free(a->vec);
		a->vec = newargs;
 	}
}

void args_push(struct args *a, char *s) {
	prepare_space(a, 1);
	a->vec[a->count++] = s;
}

void args_push2(struct args *a, char *s1, char *s2) {
	prepare_space(a, 2);
	a->vec[a->count++] = s1;
	a->vec[a->count++] = s2;
}

