// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#include <errno.h>

int errno;

char *strerror(int num)
{
	char *error;
	switch (num) {
		case 0:
			error = "Success";
			break;
		case EDOM:
			error = "Domain error";
			break;
		case ERANGE:
			error = "Range error";
			break;
		case EILSEQ:
			error = "Illegal sequence";
			break;
		case ENOENT:
			error = "No such file or directory";
			break;
		case ESPIPE:
			error = "Invalid seek";
			break;
		case EROFS:
			error = "Read-only file system";
			break;
		case EINVAL:
		case EBADF:
		case EPERM:
		case EMFILE:
		case EISCONN:
		default:
			error = "";
			break;
	}
	return error;
}

