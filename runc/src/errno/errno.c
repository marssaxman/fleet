#include "errno.h"

extern int errno;

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
		default:
			error = "";
			break;
	}
	return error;
}

