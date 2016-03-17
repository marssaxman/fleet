// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <stdlib.h>
#include <errno.h>

int system(const char *command)
{
	// If command is null, this is a query: is there a command shell?
	// There is no command shell, so we return 0. If there is a valid command,
	// we return -1 to indicate that we were unable to execute the command.
	return command? -1: 0;
}

