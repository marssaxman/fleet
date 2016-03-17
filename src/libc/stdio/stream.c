// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include "stream.h"

struct _stream *_stream_list;

void _init_stream(struct _stream *s)
{
	s->next = _stream_list;
	if (s->next) {
		s->next->back = &s->next;
	}
	s->back = &_stream_list;
	_stream_list = s;
}

void _exit_stream(struct _stream *s)
{
	*s->back = s->next;
	s->back = &s->next;
	s->next = 0;
}

